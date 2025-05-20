#pragma once

namespace vkb
{
    class Window;

    namespace core
    {
        class HPPQueue;
    }
}

namespace vkb::rendering
{
    /**
     * @brief HPPRenderContext is a transcoded version of vkb::RenderContext from vulkan to vulkan-hpp.
     *
     * See vkb::RenderContext for documentation
     */
    class HPPRenderContext
    {
        // The format to use for the RenderTargets if a swapchain isn't created
        static vk::Format DEFAULT_VK_FORMAT;

    protected:
        vkb::core::HPPDevice& device;

        const Window& window;

        // If swapchain exists, then this will be a present supported queue, else a graphics queue
        const vkb::core::HPPQueue& queue;
    };
}