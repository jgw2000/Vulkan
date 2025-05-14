#include "stdafx.h"
#include "vulkan_sample.h"
#include "core/hpp_instance.h"

namespace vkb
{
    VulkanSample::VulkanSample(const Window::Properties& properties) :
        Application(properties)
    {
    }

    VulkanSample::~VulkanSample()
    {
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

        // Creating the vulkan instance
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

        return true;
    }

    void VulkanSample::update(float delta_time)
    {
        Application::update(delta_time);
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

    core::HPPInstance& VulkanSample::get_instance()
    {
        return *instance;
    }

    const core::HPPInstance& VulkanSample::get_instance() const
    {
        return *instance;
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
}
