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
}
