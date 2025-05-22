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

        /**
         * @brief Prepares the RenderFrames for rendering
         * @param thread_count The number of threads in the application, necessary to allocate this many resource pools for each RenderFrame
         * @param create_render_target_func A function delegate, used to create a RenderTarget
         */
        void prepare(size_t thread_count = 1, HPPRenderTarget::CreateFunc create_render_target_func = HPPRenderTarget::DEFAULT_CREATE_FUNC);

    protected:
        vk::Extent2D surface_extent;

    private:
        vkb::core::HPPDevice& device;

        const Window& window;

        // If swapchain exists, then this will be a present supported queue, else a graphics queue
        const vkb::core::HPPQueue& queue;

        std::unique_ptr<vkb::core::HPPSwapchain> swapchain;

        vkb::core::HPPSwapchainProperties swapchain_properties;

        bool prepared{ false };

        // Current active frame index
        uint32_t active_frame_index{ 0 };

        // Whether a frame is active or not
        bool frame_active{ false };

        HPPRenderTarget::CreateFunc create_render_target_func = HPPRenderTarget::DEFAULT_CREATE_FUNC;

        vk::SurfaceTransformFlagBitsKHR pre_transform{ vk::SurfaceTransformFlagBitsKHR::eIdentity };

        size_t thread_count{ 1 };
    };
}
