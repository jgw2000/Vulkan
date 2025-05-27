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
}