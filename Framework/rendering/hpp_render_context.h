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

        /**
         * @brief Updates the swapchains extent and surface transform, if a swapchain exists
         * @param extent The width and height of the new swapchain images
         * @param transform The surface transform flags
         */
        void update_swapchain(const vk::Extent2D& extent, const vk::SurfaceTransformFlagBitsKHR transform);

        /**
         * @brief Recreate the RenderFrame, called after every update
         */
        void recreate();

        /**
         * @brief Prepares the next available frame for rendering
         * @param reset_mode How to reset the command buffer
         * @returns A valid command buffer to record commands to be submitted
         * Also ensures that there is an active frame if there is no existing active frame already
         */
        vkb::core::HPPCommandBuffer& begin(vkb::CommandBufferResetMode reset_mode = vkb::CommandBufferResetMode::ResetPool);

        /**
         * @brief begin_frame
         */
        void begin_frame();

        /**
         * @brief A frame is active after @ref begin_frame has been called.
         * @return The current active frame
         */
        HPPRenderFrame& get_active_frame() { return *frames[active_frame_index]; }

        /**
         * @brief A frame is active after @ref begin_frame has been called.
         * @return The current active frame index
         */
        uint32_t get_active_frame_index() { return active_frame_index; }

        /**
         * @brief A frame is active after @ref begin_frame has been called.
         * @return The previous frame
         */
        HPPRenderFrame& get_last_rendered_frame() { return *frames[active_frame_index]; }

        vkb::core::HPPDevice& get_device() { return device; }

        /**
         * @brief Returns the format that the RenderTargets are created with the HPPRenderContext
         */
        vk::Format get_format() const { return swapchain ? swapchain->get_format() : DEFAULT_VK_FORMAT; }

        const vkb::core::HPPSwapchain& get_swapchain() const { return *swapchain; }

        const vk::Extent2D& get_surface_extent() const { return surface_extent; }

        std::vector<std::unique_ptr<HPPRenderFrame>>& get_render_frames() { return frames; }

        /**
         * @brief Handles surface changes, only applicable if the render_context makes use of a swapchain
         */
        bool handle_surface_changes(bool force_update = false);

    protected:
        vk::Extent2D surface_extent;

    private:
        vkb::core::HPPDevice& device;

        const Window& window;

        // If swapchain exists, then this will be a present supported queue, else a graphics queue
        const vkb::core::HPPQueue& queue;

        std::unique_ptr<vkb::core::HPPSwapchain> swapchain;

        vkb::core::HPPSwapchainProperties swapchain_properties;

        std::vector<std::unique_ptr<HPPRenderFrame>> frames;

        vk::Semaphore acquired_semaphore;

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
