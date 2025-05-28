#include "stdafx.h"

namespace vkb::core
{
    HPPCommandBuffer::HPPCommandBuffer(HPPCommandPool& command_pool, vk::CommandBufferLevel level) :
        VulkanResource<vk::CommandBuffer>(nullptr, &command_pool.get_device()),
        command_pool{ command_pool },
        level{ level }
    {
        vk::CommandBufferAllocateInfo allocate_info{
            command_pool.get_handle(),
            level,
            1
        };

        this->set_handle(this->get_device().get_handle().allocateCommandBuffers(allocate_info).front());
    }

    HPPCommandBuffer::~HPPCommandBuffer()
    {
        // Destroy command buffer
        if (this->has_handle())
        {
            this->get_device().get_handle().freeCommandBuffers(command_pool.get_handle(), this->get_handle());
        }
    }

    void HPPCommandBuffer::begin(vk::CommandBufferUsageFlags flags, HPPCommandBuffer* primary_cmd_buf)
    {
        if (level == vk::CommandBufferLevel::eSecondary)
        {
            assert(primary_cmd_buf && "A primary command buffer pointer must be provided when calling begin from a secondary one");

            // TODO
        }
        else
        {
            return begin_impl(flags, nullptr, nullptr, 0);
        }
    }

    void HPPCommandBuffer::begin(vk::CommandBufferUsageFlags flags, const HPPRenderPass* render_pass, const HPPFramebuffer* framebuffer, uint32_t subpass_index)
    {
        begin_impl(flags, render_pass, framebuffer, subpass_index);
    }

    void HPPCommandBuffer::end()
    {
        this->get_handle().end();
    }

    vk::Result HPPCommandBuffer::reset(vkb::CommandBufferResetMode reset_mode)
    {
        assert(reset_mode == command_pool.get_reset_mode() && "Command buffer reset mode must match the one used by the pool to allocate it");

        if (reset_mode == vkb::CommandBufferResetMode::ResetIndividually)
        {
            this->get_handle().reset(vk::CommandBufferResetFlagBits::eReleaseResources);
        }

        return vk::Result::eSuccess;
    }

    void HPPCommandBuffer::image_memory_barrier(const HPPImageView& image_view, const vkb::HPPImageMemoryBarrier& memory_barrier) const
    {
        // Adjust barrier's subresource range for depth images
        auto subresource_range = image_view.get_subresource_range();
        auto format = image_view.get_format();

        if (vkb::is_depth_only_format(format))
        {
            subresource_range.aspectMask = vk::ImageAspectFlagBits::eDepth;
        }
        else if (vkb::is_depth_stencil_format(format))
        {
            subresource_range.aspectMask = vk::ImageAspectFlagBits::eDepth | vk::ImageAspectFlagBits::eStencil;
        }

        // actively ignore queue family indices provided by memory_barrier !!
        vk::ImageMemoryBarrier image_memory_barrier{
            memory_barrier.src_access_mask,
            memory_barrier.dst_access_mask,
            memory_barrier.old_layout,
            memory_barrier.new_layout,
            vk::QueueFamilyIgnored,
            vk::QueueFamilyIgnored,
            image_view.get_image().get_handle(),
            subresource_range
        };

        vk::PipelineStageFlags src_stage_mask = memory_barrier.src_stage_mask;
        vk::PipelineStageFlags dst_stage_mask = memory_barrier.dst_stage_mask;

        this->get_handle().pipelineBarrier(src_stage_mask, dst_stage_mask, {}, {}, {}, image_memory_barrier);
    }

    void HPPCommandBuffer::begin_impl(vk::CommandBufferUsageFlags flags, const HPPRenderPass* render_pass, const HPPFramebuffer* framebuffer, uint32_t subpass_index)
    {
        // TODO

        vk::CommandBufferBeginInfo begin_info{ flags };
        vk::CommandBufferInheritanceInfo inheritance;

        if (level == vk::CommandBufferLevel::eSecondary)
        {
            assert((render_pass && framebuffer) && "Render pass and framebuffer must be provided with calling begin from a secondary one");

            current_render_pass = render_pass;
            current_framebuffer = framebuffer;

            inheritance.renderPass = current_render_pass->get_handle();
            inheritance.framebuffer = current_framebuffer->get_handle();
            inheritance.subpass = subpass_index;

            begin_info.pInheritanceInfo = &inheritance;
        }

        this->get_handle().begin(begin_info);
    }
}