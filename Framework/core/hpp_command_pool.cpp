#include "stdafx.h"

namespace vkb::core
{
    HPPCommandPool::HPPCommandPool(HPPDevice& device,
                                   uint32_t                        queue_family_index,
                                   vkb::rendering::HPPRenderFrame* render_frame,
                                   size_t                          thread_index,
                                   vkb::CommandBufferResetMode     reset_mode) :
        device{ device },
        queue_family_index{ queue_family_index },
        render_frame{ render_frame },
        thread_index{ thread_index },
        reset_mode{ reset_mode }
    {
        vk::CommandPoolCreateFlags flags;
        switch (reset_mode)
        {
            case vkb::CommandBufferResetMode::ResetIndividually:
            case vkb::CommandBufferResetMode::AlwaysAllocate:
                flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer;
                break;
            case vkb::CommandBufferResetMode::ResetPool:
            default:
                flags = vk::CommandPoolCreateFlagBits::eTransient;
                break;
        }

        vk::CommandPoolCreateInfo command_pool_create_info{
            flags,
            queue_family_index
        };

        handle = device.get_handle().createCommandPool(command_pool_create_info);
    }

    HPPCommandPool::~HPPCommandPool()
    {
        // clear command buffers before destroying the command pool
        primary_command_buffers.clear();
        secondary_command_buffers.clear();

        // Destroy command pool
        if (handle)
        {
            device.get_handle().destroyCommandPool(handle);
        }
    }

    HPPCommandBuffer& HPPCommandPool::request_command_buffer(vk::CommandBufferLevel level)
    {
        if (level == vk::CommandBufferLevel::ePrimary)
        {
            if (active_primary_command_buffer_count < primary_command_buffers.size())
            {
                return primary_command_buffers[active_primary_command_buffer_count++];
            }

            primary_command_buffers.emplace_back(*this, level);
            active_primary_command_buffer_count++;
            return primary_command_buffers.back();
        }
        else
        {
            if (active_secondary_command_buffer_count < secondary_command_buffers.size())
            {
                return secondary_command_buffers[active_secondary_command_buffer_count];
            }

            secondary_command_buffers.emplace_back(*this, level);
            active_secondary_command_buffer_count++;
            return secondary_command_buffers.back();
        }
    }

    void HPPCommandPool::reset_pool()
    {
        switch (reset_mode)
        {
        case vkb::CommandBufferResetMode::ResetIndividually:
            for (auto& cmd_buf : primary_command_buffers)
            {
                cmd_buf.reset(reset_mode);
            }
            active_primary_command_buffer_count = 0;

            for (auto& cmd_buf : secondary_command_buffers)
            {
                cmd_buf.reset(reset_mode);
            }
            active_secondary_command_buffer_count = 0;
            break;

        case vkb::CommandBufferResetMode::ResetPool:
            device.get_handle().resetCommandPool(handle);
            active_primary_command_buffer_count = 0;
            active_secondary_command_buffer_count = 0;
            break;

        case vkb::CommandBufferResetMode::AlwaysAllocate:
            primary_command_buffers.clear();
            active_primary_command_buffer_count = 0;
            secondary_command_buffers.clear();
            active_secondary_command_buffer_count = 0;
            break;

        default:
            throw std::runtime_error("Unknown reset mode for command pools");
        }
    }
}