#include "stdafx.h"

namespace vkb::rendering
{
    vk::Format HPPRenderContext::DEFAULT_VK_FORMAT = vk::Format::eR8G8B8A8Srgb;

    HPPRenderContext::HPPRenderContext(vkb::core::HPPDevice&                   device,
                                      vk::SurfaceKHR                           surface,
                                      const vkb::Window&                       window,
                                      vk::PresentModeKHR                       present_mode,
                                      const std::vector<vk::PresentModeKHR>&   present_mode_priority_list,
                                      const std::vector<vk::SurfaceFormatKHR>& surface_format_priority_list) :
        device{ device }, window{ window }, queue{ device.get_suitable_graphics_queue() }, surface_extent{window.get_extent().width, window.get_extent().height}
    {
        if (surface)
        {
            auto surface_properties = device.get_gpu().get_handle().getSurfaceCapabilitiesKHR(surface);

            if (surface_properties.currentExtent.width = 0xFFFFFFFF)
            {
                swapchain =
                    std::make_unique<vkb::core::HPPSwapchain>(device, surface, present_mode, present_mode_priority_list, surface_format_priority_list, surface_extent);
            }
            else
            {
                swapchain = std::make_unique<vkb::core::HPPSwapchain>(device, surface, present_mode, present_mode_priority_list, surface_format_priority_list);
            }
        }
    }

    void HPPRenderContext::prepare(size_t thread_count, HPPRenderTarget::CreateFunc create_render_target_func)
    {
        device.get_handle().waitIdle();

        if (swapchain)
        {
            surface_extent = swapchain->get_extent();

            vk::Extent3D extent{ surface_extent.width, surface_extent.height, 1 };

            for (auto& image_handle : swapchain->get_images())
            {
                auto swapchain_image = core::HPPImage{ device, image_handle, extent, swapchain->get_format(), swapchain->get_usage() };
                auto render_target = create_render_target_func(std::move(swapchain_image));
                frames.emplace_back(std::make_unique<HPPRenderFrame>(device, std::move(render_target), thread_count));
            }
        }
        else
        {
            // Otherwise, create a single RenderFrame
            swapchain = nullptr;

            auto color_image = vkb::core::HPPImage{
                device,
                vk::Extent3D{surface_extent.width, surface_extent.height, 1},
                DEFAULT_VK_FORMAT,      // We can use any format here that we like
                vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eTransferSrc,
                VMA_MEMORY_USAGE_GPU_ONLY
            };

            auto render_target = create_render_target_func(std::move(color_image));
            frames.emplace_back(std::make_unique<HPPRenderFrame>(device, std::move(render_target), thread_count));
        }

        this->create_render_target_func = create_render_target_func;
        this->thread_count              = thread_count;
        this->prepared                  = true;
    }

    vkb::core::HPPCommandBuffer& HPPRenderContext::begin(vkb::CommandBufferResetMode reset_mode)
    {
        assert(prepared && "HPPRenderContext not prepared for rendering, call prepare()");

        if (!frame_active)
        {
            begin_frame();
        }

        if (!acquired_semaphore)
        {
            throw std::runtime_error("Couldn't begin frame");
        }

        const auto& queue = device.get_queue_by_flags(vk::QueueFlagBits::eGraphics, 0);

        return get_active_frame().request_command_buffer(queue, reset_mode);
    }

    void HPPRenderContext::update_swapchain(const vk::Extent2D& extent, const vk::SurfaceTransformFlagBitsKHR transform)
    {
        if (!swapchain)
        {
            return;
        }

        auto width = extent.width;
        auto height = extent.height;
        if ((transform & vk::SurfaceTransformFlagBitsKHR::eRotate90) || (transform & vk::SurfaceTransformFlagBitsKHR::eRotate270))
        {
            // Pre-rotation: always use native orientation i.e. if rotated, use width and height of identity transform
            std::swap(width, height);
        }

        swapchain = std::make_unique<vkb::core::HPPSwapchain>(*swapchain, vk::Extent2D{ width, height }, transform);

        // Save the preTransform attribute for future rotations
        pre_transform = transform;

        recreate();
    }

    void HPPRenderContext::recreate()
    {
        vk::Extent2D swapchain_extent = swapchain->get_extent();
        vk::Extent3D extent{ swapchain_extent.width, swapchain_extent.height, 1 };

        auto frame_it = frames.begin();
        for (auto& image_handle : swapchain->get_images())
        {
            vkb::core::HPPImage swapchain_image{ device, image_handle, extent, swapchain->get_format(), swapchain->get_usage() };
            auto render_target = create_render_target_func(std::move(swapchain_image));

            if (frame_it != frames.end())
            {
                (*frame_it)->update_render_target(std::move(render_target));
            }
            else
            {
                // Create a new frame if the new swapchain has more images than current frames
                frames.emplace_back(std::make_unique<HPPRenderFrame>(device, std::move(render_target), thread_count));
            }

            ++frame_it;
        }

        device.get_resource_cache().clear_framebuffers();
    }

    void HPPRenderContext::begin_frame()
    {
        // Only handle surface changes if a swapchain exists
        if (swapchain)
        {
            handle_surface_changes();
        }

        assert(!frame_active && "Frame is still active, please call end_frame");

        auto& prev_frame = *frames[active_frame_index];

        // We will use the acquired semaphore in a different frame context.
        // so we need to hold ownership.
        acquired_semaphore = prev_frame.request_semaphore_with_ownership();

        if (swapchain)
        {
            vk::Result result;
            try
            {
                std::tie(result, active_frame_index) = swapchain->acquire_next_image(acquired_semaphore);
            }
            catch (vk::OutOfDateKHRError&)
            {
                result = vk::Result::eErrorOutOfDateKHR;
            }

            if (result == vk::Result::eSuboptimalKHR || result == vk::Result::eErrorOutOfDateKHR)
            {
                bool swapchain_updated = handle_surface_changes(result == vk::Result::eErrorOutOfDateKHR);
                if (swapchain_updated)
                {
                    // Need to destroy and reallocate acquired_semaphore since it may have already been signaled
                    device.get_handle().destroySemaphore(acquired_semaphore);
                    acquired_semaphore = prev_frame.request_semaphore_with_ownership();
                    std::tie(result, active_frame_index) = swapchain->acquire_next_image(acquired_semaphore);
                }
            }

            if (result != vk::Result::eSuccess)
            {
                prev_frame.reset();
                return;
            }
        }

        // Now the frame is active again
        frame_active = true;

        // Wait on all resource to be freed from the previous render to this frame
        wait_frame();
    }

    void HPPRenderContext::wait_frame()
    {
        get_active_frame().reset();
    }

    void HPPRenderContext::end_frame(vk::Semaphore semaphore)
    {
        assert(frame_active && "Frame is not active, please call begin_frame");

        if (swapchain)
        {
            vk::SwapchainKHR vk_swapchain = swapchain->get_handle();
            vk::PresentInfoKHR present_info{
                1,
                &semaphore,
                1,
                &vk_swapchain,
                &active_frame_index
            };

            vk::Result result;
            try
            {
                result = queue.present(present_info);
            }
            catch (vk::OutOfDateKHRError&)
            {
                result = vk::Result::eErrorOutOfDateKHR;
            }

            if (result == vk::Result::eSuboptimalKHR || result == vk::Result::eErrorOutOfDateKHR)
            {
                handle_surface_changes();
            }
        }

        // Frame is not active anymore
        if (acquired_semaphore)
        {
            release_owned_semaphore(acquired_semaphore);
            acquired_semaphore = nullptr;
        }

        frame_active = false;
    }

    void HPPRenderContext::submit(vkb::core::HPPCommandBuffer& command_buffer)
    {
        submit({ &command_buffer });
    }

    void HPPRenderContext::submit(const std::vector<vkb::core::HPPCommandBuffer*>& command_buffers)
    {
        assert(frame_active && "HPPRenderContext is inactive, cannot submit command buffer. Please call begin()");

        vk::Semaphore render_semaphore;

        if (swapchain)
        {
            assert(acquired_semaphore && "We do not have acquired_semaphore, it was probably consumed?");
            render_semaphore = submit(queue, command_buffers, acquired_semaphore, vk::PipelineStageFlagBits::eColorAttachmentOutput);
        }
        else
        {
            submit(queue, command_buffers);
        }

        end_frame(render_semaphore);
    }

    void HPPRenderContext::submit(const vkb::core::HPPQueue& queue, const std::vector<vkb::core::HPPCommandBuffer*>& command_buffers)
    {
        std::vector<vk::CommandBuffer> cmd_buf_handles(command_buffers.size(), nullptr);
        std::ranges::transform(command_buffers, cmd_buf_handles.begin(), [](const vkb::core::HPPCommandBuffer* cmd_buf) { return cmd_buf->get_handle(); });

        auto& frame = get_active_frame();

        vk::SubmitInfo submit_info = {};
        submit_info.commandBufferCount = static_cast<uint32_t>(cmd_buf_handles.size());
        submit_info.pCommandBuffers = cmd_buf_handles.data();

        auto fence = frame.request_fence();
        queue.get_handle().submit(submit_info, fence);
    }

    vk::Semaphore HPPRenderContext::submit(const vkb::core::HPPQueue&                       queue,
                                           const std::vector<vkb::core::HPPCommandBuffer*>& command_buffers,
                                           vk::Semaphore                                    wait_semaphore,
                                           vk::PipelineStageFlags                           wait_pipeline_stage)
    {
        std::vector<vk::CommandBuffer> cmd_buf_handles(command_buffers.size(), nullptr);
        std::ranges::transform(command_buffers, cmd_buf_handles.begin(), [](const vkb::core::HPPCommandBuffer* cmd_buf) { return cmd_buf->get_handle(); });

        auto& frame = get_active_frame();
        auto signal_semaphore = frame.request_semaphore();

        vk::SubmitInfo submit_info = {};
        submit_info.commandBufferCount = static_cast<uint32_t>(cmd_buf_handles.size());
        submit_info.pCommandBuffers = cmd_buf_handles.data();
        submit_info.signalSemaphoreCount = 1;
        submit_info.pSignalSemaphores = &signal_semaphore;

        if (wait_semaphore)
        {
            submit_info.setWaitSemaphores(wait_semaphore);
            submit_info.pWaitDstStageMask = &wait_pipeline_stage;
        }

        auto fence = frame.request_fence();
        queue.get_handle().submit(submit_info, fence);

        return signal_semaphore;
    }

    void HPPRenderContext::release_owned_semaphore(vk::Semaphore semaphore)
    {
        get_active_frame().release_owned_semaphore(semaphore);
    }

    bool HPPRenderContext::handle_surface_changes(bool force_update)
    {
        if (!swapchain)
        {
            return false;
        }

        auto surface_properties = device.get_gpu().get_handle().getSurfaceCapabilitiesKHR(swapchain->get_surface());
        if (surface_properties.currentExtent.width == 0xFFFFFFFF)
        {
            return false;
        }

        // Only recreate the swapchain if the dimension have changed;
        // handle_surface_changes() is called on VK_SUBOPTIMAL_KHR,
        // which might not be due to a surface resize
        if (surface_properties.currentExtent.width != surface_extent.width ||
            surface_properties.currentExtent.height != surface_extent.height ||
            force_update)
        {
            // Recreate swapchain
            device.get_handle().waitIdle();

            update_swapchain(surface_properties.currentExtent, pre_transform);
            surface_extent = surface_properties.currentExtent;
            return true;
        }

        return false;
    }
}
