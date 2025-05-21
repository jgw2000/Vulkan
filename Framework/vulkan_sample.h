#pragma once

#include "platform/application.h"
#include "platform/window.h"

namespace vkb
{
    /**
     * @mainpage Overview of the framework
     *
     * @section initialization Initialization
     *
     * @subsection platform_init Platform initialization
     * The lifecycle of a Vulkan sample starts by instantiating the correct Platform
     * (e.g. WindowsPlatform) and then calling initialize() on it, which sets up
     * the windowing system and logging. Then it calls the parent Platform::initialize(),
     * which takes ownership of the active application. It's the platforms responsibility
     * to then call VulkanSample::prepare() to prepare the vulkan sample when it is ready.
     *
     * @subsection sample_init Sample initialization
     * The preparation step is divided in two steps, one in VulkanSample and the other in the
     * specific sample, such as SurfaceRotation.
     * VulkanSample::prepare() contains functions that do not require customization,
     * including creating a Vulkan instance, the surface and getting physical devices.
     * The prepare() function for the specific sample completes the initialization, including:
     * - setting enabled Stats
     * - creating the Device
     * - creating the Swapchain
     * - creating the RenderContext (or child class)
     * - preparing the RenderContext
     * - loading the sg::Scene
     * - creating the RenderPipeline with ShaderModule (s)
     * - creating the sg::Camera
     * - creating the Gui
     *
     * @section frame_rendering Frame rendering
     *
     * @subsection update Update function
     * Rendering happens in the update() function. Each sample can override it, e.g.
     * to recreate the Swapchain in SwapchainImages when required by user input.
     * Typically a sample will then call VulkanSample::update().
     *
     * @subsection rendering Rendering
     * A series of steps are performed, some of which can be customized (it will be
     * highlighted when that's the case):
     *
     * - calling sg::Script::update() for all sg::Script (s)
     * - beginning a frame in RenderContext (does the necessary waiting on fences and
     *   acquires an core::Image)
     * - requesting a CommandBuffer
     * - updating Stats and Gui
     * - getting an active RenderTarget constructed by the factory function of the RenderFrame
     * - setting up barriers for color and depth, note that these are only for the default RenderTarget
     * - calling VulkanSample::draw_swapchain_renderpass (see below)
     * - setting up a barrier for the Swapchain transition to present
     * - submitting the CommandBuffer and end the Frame (present)
     *
     * @subsection draw_swapchain Draw swapchain renderpass
     * The function starts and ends a RenderPass which includes setting up viewport, scissors,
     * blend state (etc.) and calling draw_scene.
     * Note that RenderPipeline::draw is not virtual in RenderPipeline, but internally it calls
     * Subpass::draw for each Subpass, which is virtual and can be customized.
     *
     * @section framework_classes Main framework classes
     *
     * - RenderContext
     * - RenderFrame
     * - RenderTarget
     * - RenderPipeline
     * - ShaderModule
     * - ResourceCache
     * - BufferPool
     * - Core classes: Classes in vkb::core wrap Vulkan objects for indexing and hashing.
     */

    namespace core
    {
        class HPPInstance;
        class HPPPhysicalDevice;
        class HPPDevice;
    }

    namespace rendering
    {
        class HPPRenderContext;
    }

    class VulkanSample : public vkb::Application
    {
        /// <summary>
        /// PUBLIC INTERFACE
        /// </summary>
    public:
        VulkanSample(const Window::Properties& properties);
        ~VulkanSample() override;

        /// <summary>
        /// PROTECTED VIRTUAL INTERFACE
        /// </summary>
    protected:
        /**
        * @brief Additional sample initialization
        */
        bool prepare() override;

        /**
        * @brief Main loop sample events
        */
        void update(float delta_time) override;

        void finish() override;
        bool resize(uint32_t width, uint32_t height) override;

        /**
         * @brief Request features from the gpu based on what is supported
         */
        virtual void request_gpu_features(core::HPPPhysicalDevice& gpu) {}

        /**
         * @brief Set the Vulkan API version to request at instance creation time
         */
        void set_api_version(uint32_t requested_api_version) { api_version = requested_api_version; }

        /**
         * @brief Sets whether or not the first graphics queue should have higher priority than other queues.
         * Very specific feature which is used by async compute samples.
         * Needs to be called before prepare().
         * @param enable If true, present queue will have prio 1.0 and other queues have prio 0.5.
         * Default state is false, where all queues have 0.5 priority.
         */
        void set_high_priority_graphics_queue_enable(bool enable) { high_priority_graphics_queue = enable; }

        /**
         * @brief Create the Vulkan instance used by this sample
         * @note Can be overridden to implement custom instance creation
         */
        virtual std::unique_ptr<core::HPPInstance> create_instance();

        /**
         * @brief Create the Vulkan device used by this sample
         * @note Can be overridden to implement custom device creation
         */
        virtual std::unique_ptr<core::HPPDevice> create_device(core::HPPPhysicalDevice& gpu);

        /**
         * @brief Override this to customise the creation of the render_context
         */
        virtual void create_render_context();

        /**
         * @brief Override this to customise the creation of the swapchain and render_context
         */
        virtual void prepare_render_context();

        /**
         * @brief Add a sample-specific device extension
         * @param extension The extension name
         * @param optional (Optional) Whether the extension is optional
         */
        void add_device_extension(const char* extension);

        /**
         * @brief Add a sample-specific instance extension
         * @param extension The extension name
         */
        void add_instance_extension(const char* extension);

        /**
         * @brief Add a sample-specific instance layer
         * @param layer The layer name
         */
        void add_instance_layer(const char* layer);

        /**
         * @brief Add a sample-specific layer setting
         * @param layerSetting The layer setting
         */
        void add_layer_setting(const vk::LayerSettingEXT& layerSetting);

        /**
         * @brief A helper to create a render context
         */
        void create_render_context(const std::vector<vk::SurfaceFormatKHR>& surface_priority_list);

        core::HPPInstance&                 get_instance()             { return *instance; }
        const core::HPPInstance&           get_instance() const       { return *instance; }
        core::HPPDevice&                   get_device()               { return *device; }
        const core::HPPDevice&             get_device() const         { return *device; }
        rendering::HPPRenderContext&       get_render_context()       { return *render_context; }
        const rendering::HPPRenderContext& get_render_context() const { return *render_context; }

        /// <summary>
        /// PRIVATE INTERFACE
        /// </summary>
    private:
        void create_render_context_impl(const std::vector<vk::SurfaceFormatKHR>& surface_priority_list);

        /**
         * @brief Get sample-specific device extensions.
         *
         * @return Vector of device extensions. Default is empty vector.
         */
        const std::vector<const char*>& get_device_extensions() const;

        /**
         * @brief Get sample-specific instance extensions.
         *
         * @return Vector of instance extensions. Default is empty vector.
         */
        const std::vector<const char*>& get_instance_extensions() const;

        /**
         * @brief Get sample-specific instance layers.
         *
         * @return Vector of instance layers. Default is empty vector.
         */
        const std::vector<const char*>& get_instance_layers() const;

        /**
         * @brief Get sample-specific layer settings.
         *
         * @return Vector of layer settings. Default is empty vector.
         */
        const std::vector<vk::LayerSettingEXT>& get_layer_settings() const;

        /// <summary>
        /// PRIVATE MEMBERS
        /// </summary>
    private:
        /**
         * @brief The Vulkan instance
         */
        std::unique_ptr<core::HPPInstance> instance;

        /**
         * @brief The Vulkan device
         */
        std::unique_ptr<core::HPPDevice> device;

        /**
         * @brief Context used for rendering, it is responsible for managing the frames and their underlying images
         */
        std::unique_ptr<rendering::HPPRenderContext> render_context;

        /**
         * @brief The Vulkan surface
         */
        vk::SurfaceKHR surface;

        /**
         * @brief A list of surface formats in order of priority (vector[0] has high priority, vector[size-1] has low priority)
         */
        std::vector<vk::SurfaceFormatKHR> surface_priority_list = {
            { vk::Format::eR8G8B8A8Srgb, vk::ColorSpaceKHR::eSrgbNonlinear },
            { vk::Format::eB8G8R8A8Srgb, vk::ColorSpaceKHR::eSrgbNonlinear },
        };

        /** @brief Vector of device extensions to be enabled for this example(must be set in the derived constructor) */
        std::vector<const char*> device_extensions;


        /** @brief Vector of instance extensions to be enabled for this example (must be set in the derived constructor) */
        std::vector<const char*> instance_extensions;

        /** @brief Vector of instance layers to be enabled for this example (must be set in the derived constructor) */
        std::vector<const char*> instance_layers;

        /** @brief Vector of layer settings to be enabled for this example (must be set in the derived constructor) */
        std::vector<vk::LayerSettingEXT> layer_settings;

        /** @brief The Vulkan API version to request for this sample at instance creation time */
        uint32_t api_version = VK_API_VERSION_1_0;

        /** @brief Whether or not we want a high priority graphics queue. */
        bool high_priority_graphics_queue{ false };
    };
}
