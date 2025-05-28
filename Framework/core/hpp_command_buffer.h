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

        /**
         * @brief Sets the command buffer so that it is ready for recording
         *        If it is a secondary command buffer, a pointer to the
         *        primary command buffer it inherits from must be provided
         * @param flags Usage behavior for the command buffer
         * @param primary_cmd_buf (optional)
         */
        void begin(vk::CommandBufferUsageFlags flags, HPPCommandBuffer* primary_cmd_buf = nullptr);

        /**
         * @brief Sets the command buffer so that it is ready for recording
         *        If it is a secondary command buffer, pointers to the
         *        render pass and framebuffer as well as subpass index must be provided
         * @param flags Usage behavior for the command buffer
         * @param render_pass
         * @param framebuffer
         * @param subpass_index
         */
        void begin(vk::CommandBufferUsageFlags flags, const HPPRenderPass* render_pass, const HPPFramebuffer* framebuffer, uint32_t subpass_index);

        void end();

        /**
         * @brief Reset the command buffer to a state where it can be recorded to
         * @param reset_mode How to reset the buffer, should match the one used by the pool to allocate it
         */
        vk::Result reset(vkb::CommandBufferResetMode reset_mode);

        void image_memory_barrier(const HPPImageView& image_view, const vkb::HPPImageMemoryBarrier& memory_barrier) const;

    private:
        void begin_impl(vk::CommandBufferUsageFlags flags, const HPPRenderPass* render_pass, const HPPFramebuffer* framebuffer, uint32_t subpass_index);

    private:
        HPPCommandPool&              command_pool;
        const HPPRenderPass*         current_render_pass = nullptr;
        const HPPFramebuffer*        current_framebuffer = nullptr;
        const vk::CommandBufferLevel level = {};
    };
}