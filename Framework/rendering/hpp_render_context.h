#pragma once

namespace vkb::rendering
{
    /**
     * @brief HPPRenderContext is a transcoded version of vkb::RenderContext from vulkan to vulkan-hpp.
     *
     * See vkb::RenderContext for documentation
     */
    class HPPRenderContext
    {
    public:
        // The format to use for the RenderTargets if a swapchain isn't created
        static vk::Format DEFAULT_VK_FORMAT;

        /**
         * @brief Constructor
         * @param device A valid device
         * @param surface A surface, nullptr if in offscreen mode
         * @param window The window where the surface was created
         * @param present_mode Requests to set the present mode of the swapchain
         * @param present_mode_priority_list The order in which the swapchain prioritizes selecting its present mode
         * @param surface_format_priority_list The order in which the swapchain prioritizes selecting its surface format
         */
        HPPRenderContext(vkb::core::HPPDevice&                    device,
                         vk::SurfaceKHR                           surface,
                         const vkb::Window&                       window,
                         vk::PresentModeKHR                       present_mode                 = vk::PresentModeKHR::eFifo,
                         const std::vector<vk::PresentModeKHR>&   present_mode_priority_list   = { vk::PresentModeKHR::eFifo, vk::PresentModeKHR::eMailbox },
                         const std::vector<vk::SurfaceFormatKHR>& surface_format_priority_list = {
                            { vk::Format::eR8G8B8A8Srgb, vk::ColorSpaceKHR::eSrgbNonlinear }, { vk::Format::eB8G8R8A8Srgb, vk::ColorSpaceKHR::eSrgbNonlinear } });
        ~HPPRenderContext() = default;
        
        HPPRenderContext(const HPPRenderContext&) = delete;
        HPPRenderContext(HPPRenderContext&&) = delete;

        HPPRenderContext& operator=(const HPPRenderContext&) = delete;
        HPPRenderContext& operator=(HPPRenderContext&&) = delete;

    protected:
        vk::Extent2D surface_extent;

    private:
        vkb::core::HPPDevice& device;

        const Window& window;

        // If swapchain exists, then this will be a present supported queue, else a graphics queue
        const vkb::core::HPPQueue& queue;

        std::unique_ptr<vkb::core::HPPSwapchain> swapchain;

        vkb::core::HPPSwapchainProperties swapchain_properties;
    };
}
