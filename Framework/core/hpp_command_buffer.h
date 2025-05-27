#pragma once

namespace vkb::core
{
    /**
     * @brief Helper class to manage and record a command buffer, building and
     *        keeping track of pipeline state and resource bindings
     */
    class HPPCommandBuffer : public VulkanResource<vk::CommandBuffer>
    {
    public:
        HPPCommandBuffer(HPPCommandPool& command_pool, vk::CommandBufferLevel level);
        ~HPPCommandBuffer();

        HPPCommandBuffer(const HPPCommandBuffer&) = delete;
        HPPCommandBuffer(HPPCommandBuffer&&) = default;

        HPPCommandBuffer& operator=(const HPPCommandBuffer&) = delete;
        HPPCommandBuffer& operator=(HPPCommandBuffer&&) = default;

    private:
        HPPCommandPool& command_pool;
        const vk::CommandBufferLevel level = {};
    };
}