#include "stdafx.h"
#include "vulkan_sample.h"

namespace vkb
{
    VulkanSample::VulkanSample(const Window::Properties& properties) :
        Application(properties)
    {
    }

    VulkanSample::~VulkanSample()
    {
        if (device)
        {
            device->get_handle().waitIdle();
        }

        render_context.reset();
        device.reset();

        if (surface)
        {
            instance->get_handle().destroySurfaceKHR(surface);
        }

        instance.reset();
    }

    bool VulkanSample::prepare()
    {
        if (!Application::prepare())
        {
            return false;
        }

        static vk::detail::DynamicLoader dl;
        VULKAN_HPP_DEFAULT_DISPATCHER.init(dl.getProcAddress<PFN_vkGetInstanceProcAddr>("vkGetInstanceProcAddr"));

        if (volkInitialize() != VK_SUCCESS)
        {
            throw std::runtime_error("volkInitialize failed.");
        }

        // 1. Creating the vulkan instance
        for (const char* extension_name : window->get_required_surface_extensions())
        {
            add_instance_extension(extension_name);
        }

#if defined(_DEBUG)
        auto available_instance_extensions = vk::enumerateInstanceExtensionProperties();
        auto debugExtensionIt = std::ranges::find_if(
            available_instance_extensions,
            [](const vk::ExtensionProperties& ep) {return strcmp(ep.extensionName, VK_EXT_DEBUG_UTILS_EXTENSION_NAME) == 0; }
        );

        if (debugExtensionIt != available_instance_extensions.end())
        {
            add_instance_extension(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        }
#endif

        instance = create_instance();

        // 2. Getting a valid vulkan surface from the platform
        surface = static_cast<vk::SurfaceKHR>(window->create_surface(static_cast<VkInstance>(instance->get_handle())));
        if (!surface)
        {
            throw std::runtime_error("Failed to create window surface.");
        }

        auto& gpu = instance->get_suitable_gpu(surface);
        gpu.set_high_priority_graphics_queue_enable(high_priority_graphics_queue);

        // Request to enable ASTC
        if (gpu.get_features().textureCompressionASTC_LDR)
        {
            gpu.get_mutable_requested_features().textureCompressionASTC_LDR = true;
        }

        // Request sample required GPU features
        request_gpu_features(gpu);

        // Creating vulkan device, specifying the swapchain extension always
        add_device_extension(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

        // 3. Create logical device
        device = create_device(gpu);

        // 4. Create swapchain and render context
        create_render_context();
        prepare_render_context();

        // TODO

        return true;
    }

    void VulkanSample::update(float delta_time)
    {
        Application::update(delta_time);

        auto& command_buffer = render_context->begin();
    }

    void VulkanSample::finish()
    {
        Application::finish();
    }

    bool VulkanSample::resize(uint32_t width, uint32_t height)
    {
        if (!Application::resize(width, height))
        {
            return false;
        }

        return true;
    }

    std::unique_ptr<core::HPPInstance> VulkanSample::create_instance()
    {
        return std::make_unique<core::HPPInstance>(get_name(), get_instance_extensions(), get_instance_layers(), get_layer_settings(), api_version);
    }

    std::unique_ptr<core::HPPDevice> VulkanSample::create_device(core::HPPPhysicalDevice& gpu)
    {
        return std::make_unique<core::HPPDevice>(gpu, surface, get_device_extensions());
    }

    void VulkanSample::create_render_context()
    {
        create_render_context_impl(surface_priority_list);
    }

    void VulkanSample::prepare_render_context()
    {
        render_context->prepare();
    }

    void VulkanSample::add_device_extension(const char* extension)
    {
        device_extensions.emplace_back(extension);
    }

    void VulkanSample::add_instance_extension(const char* extension)
    {
        instance_extensions.emplace_back(extension);
    }

    void VulkanSample::add_instance_layer(const char* layer)
    {
        instance_layers.emplace_back(layer);
    }

    void VulkanSample::add_layer_setting(const vk::LayerSettingEXT& layerSetting)
    {
        layer_settings.emplace_back(layerSetting);
    }

    void VulkanSample::create_render_context(const std::vector<vk::SurfaceFormatKHR>& surface_priority_list)
    {
        create_render_context_impl(surface_priority_list);
    }

    const std::vector<const char*>& VulkanSample::get_device_extensions() const
    {
        return device_extensions;
    }

    const std::vector<const char*>& VulkanSample::get_instance_extensions() const
    {
        return instance_extensions;
    }

    const std::vector<const char*>& VulkanSample::get_instance_layers() const
    {
        return instance_layers;
    }

    const std::vector<vk::LayerSettingEXT>& VulkanSample::get_layer_settings() const
    {
        return layer_settings;
    }

    void VulkanSample::set_render_context(std::unique_ptr<rendering::HPPRenderContext>&& rc)
    {
        render_context.reset(rc.release());
    }

    void VulkanSample::set_render_pipeline(std::unique_ptr<rendering::HPPRenderPipeline>&& rp)
    {
        render_pipeline.reset(rp.release());
    }

    void VulkanSample::create_render_context_impl(const std::vector<vk::SurfaceFormatKHR>& surface_priority_list)
    {
        auto present_mode               = (window->get_properties().vsync == Window::Vsync::ON) ? vk::PresentModeKHR::eFifo : vk::PresentModeKHR::eMailbox;
        auto present_mode_priority_list = { vk::PresentModeKHR::eFifo, vk::PresentModeKHR::eImmediate, vk::PresentModeKHR::eMailbox };
    
        render_context =
            std::make_unique<vkb::rendering::HPPRenderContext>(*device, surface, *window, present_mode, present_mode_priority_list, surface_priority_list);
    }
}
