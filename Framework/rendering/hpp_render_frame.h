#pragma once

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
    private:
        vkb::core::HPPDevice& device;
        
        size_t thread_count;

        std::unique_ptr<HPPRenderTarget> swapchain_render_target;
    };
}