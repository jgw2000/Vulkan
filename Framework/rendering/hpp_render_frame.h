#pragma once

#include "hpp_fence_pool.h"
#include "hpp_semaphore_pool.h"

namespace vkb::rendering
{
    /**
     * @brief HPPRenderFrame is a transcoded version of vkb::RenderFrame from vulkan to vulkan-hpp.
     *
     * See vkb::HPPRenderFrame for documentation
     */
     /**
      * @brief HPPRenderFrame is a container for per-frame data, including BufferPool objects,
      * synchronization primitives (semaphores, fences) and the swapchain RenderTarget.
      *
      * When creating a RenderTarget, we need to provide images that will be used as attachments
      * within a RenderPass. The HPPRenderFrame is responsible for creating a RenderTarget using
      * RenderTarget::CreateFunc. A custom RenderTarget::CreateFunc can be provided if a different
      * render target is required.
      *
      * A HPPRenderFrame cannot be destroyed individually since frames are managed by the RenderContext,
      * the whole context must be destroyed. This is because each HPPRenderFrame holds Vulkan objects
      * such as the swapchain image.
      */
    class HPPRenderFrame
    {
    public:
        HPPRenderFrame(vkb::core::HPPDevice& device, std::unique_ptr<HPPRenderTarget>&& render_target, size_t thread_count = 1);

        HPPRenderFrame(const HPPRenderFrame&) = delete;
        HPPRenderFrame(HPPRenderFrame&&) = delete;
        HPPRenderFrame& operator=(const HPPRenderFrame&) = delete;
        HPPRenderFrame& operator=(HPPRenderFrame&&) = delete;

        vkb::core::HPPDevice&        get_device()               { return device; }
        const vkb::HPPFencePool&     get_fence_pool() const     { return fence_pool; }
        const vkb::HPPSemaphorePool& get_semaphore_pool() const { return semaphore_pool; }
        HPPRenderTarget&             get_render_target()        { return *swapchain_render_target; }
        const HPPRenderTarget&       get_render_target() const  { return *swapchain_render_target; }

        vk::Fence     request_fence();
        vk::Semaphore request_semaphore();
        vk::Semaphore request_semaphore_with_ownership();
        void          reset();

        void release_owned_semaphore(vk::Semaphore semaphore);

        /**
         * @brief Requests a command buffer to the command pool of the active frame
         *        A frame should be active at the moment of requesting it
         * @param queue The queue command buffers will be submitted on
         * @param reset_mode Indicate how the command buffer will be used, may trigger a
         *        pool re-creation to set necessary flags
         * @param level Command buffer level, either primary or secondary
         * @param thread_index Selects the thread's command pool used to manage the buffer
         * @return A command buffer related to the current active frame
         */
        vkb::core::HPPCommandBuffer& request_command_buffer(
            const vkb::core::HPPQueue&  queue,
            vkb::CommandBufferResetMode reset_mode   = vkb::CommandBufferResetMode::ResetPool,
            vk::CommandBufferLevel      level        = vk::CommandBufferLevel::ePrimary,
            size_t                      thread_index = 0
        );

        /**
         * @brief Called when the swapchain changes
         * @param render_target A new render target with updated images
         */
        void update_render_target(std::unique_ptr<HPPRenderTarget>&& render_target);

    private:
        /**
         * @brief Retrieve the frame's command pool(s)
         * @param queue The queue command buffers will be submitted on
         * @param reset_mode Indicate how the command buffers will be reset after execution,
         *        may trigger a pool re-creation to set necessary flags
         * @return The frame's command pool(s)
         */
        std::vector<std::unique_ptr<vkb::core::HPPCommandPool>>& get_command_pools(
            const vkb::core::HPPQueue& queue,
            vkb::CommandBufferResetMode reset_mode
        );

    private:
        vkb::core::HPPDevice& device;

        // Command pools associated with the frame
        std::map<uint32_t, std::vector<std::unique_ptr<vkb::core::HPPCommandPool>>> command_pools;

        vkb::HPPFencePool fence_pool;
        vkb::HPPSemaphorePool semaphore_pool;
        
        size_t thread_count;
        std::unique_ptr<HPPRenderTarget> swapchain_render_target;
    };
}