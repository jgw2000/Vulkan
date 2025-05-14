#include "stdafx.h"
#include "hpp_instance.h"

#if defined(_DEBUG)
#   define USE_VALIDATION_LAYERS
#   define USE_VALIDATION_LAYER_FEATURES
#endif

namespace vkb
{
    namespace
    {
#ifdef USE_VALIDATION_LAYERS
        VKAPI_ATTR vk::Bool32 VKAPI_CALL debug_utils_messenger_callback(
            vk::DebugUtilsMessageSeverityFlagBitsEXT      message_severity,
            vk::DebugUtilsMessageTypeFlagsEXT             message_type,
            const vk::DebugUtilsMessengerCallbackDataEXT* callback_data,
            void*                                         user_data
        )
        {
            return VK_FALSE;
        }

        static VKAPI_ATTR vk::Bool32 VKAPI_CALL debug_callback(
            vk::DebugReportFlagsEXT flags,
            vk::DebugReportObjectTypeEXT /*type*/,
            uint64_t /*object*/,
            size_t /*location*/,
            int32_t /*message_code*/,
            const char* layer_prefix,
            const char* message,
            void* /*user_data*/
        )
        {
            return VK_FALSE;
        }
#endif
    }

    bool validate_layers(const std::vector<const char*>&         required,
                         const std::vector<vk::LayerProperties>& available)
    {
        for (auto layer : required)
        {
            bool found = false;
            for (auto& available_layer : available)
            {
                if (strcmp(available_layer.layerName, layer) == 0)
                {
                    found = true;
                    break;
                }
            }

            if (!found)
            {
                return false;
            }
        }

        return true;
    }
}

namespace vkb::core
{
    namespace
    {
        bool enable_extension(const char*                                 requested_extension,
                              const std::vector<vk::ExtensionProperties>& available_extensions,
                              std::vector<const char*>&                   enabled_extensions)
        {
            bool is_available =
                std::ranges::any_of(available_extensions,
                                    [&requested_extension](const auto& available_extension) { return strcmp(requested_extension, available_extension.extensionName) == 0; });
            if (is_available)
            {
                bool is_already_enabled =
                    std::ranges::any_of(enabled_extensions,
                                        [&requested_extension](const auto& enabled_extension) { return strcmp(requested_extension, enabled_extension) == 0; });
                if (!is_already_enabled)
                {
                    enabled_extensions.emplace_back(requested_extension);
                }
            }

            return is_available;
        }

        bool enable_layer(const char*                             requested_layer,
                          const std::vector<vk::LayerProperties>& available_layers,
                          std::vector<const char*>&               enabled_layers)
        {
            bool is_available =
                std::ranges::any_of(available_layers,
                                    [&requested_layer](const auto& available_layer) { return strcmp(requested_layer, available_layer.layerName) == 0; });
            if (is_available)
            {
                bool is_already_enabled =
                    std::ranges::any_of(enabled_layers,
                                        [&requested_layer](const auto& enabled_layer) { return strcmp(requested_layer, enabled_layer) == 0; });
                if (!is_already_enabled)
                {
                    enabled_layers.emplace_back(requested_layer);
                }
            }

            return is_available;
        }
    }

    HPPInstance::HPPInstance(const std::string&                      application_name,
                             const std::vector<const char*>&         requested_extensions,
                             const std::vector<const char*>&         requested_layers,
                             const std::vector<vk::LayerSettingEXT>& required_layer_settings,
                             uint32_t                                api_version)
    {
        auto available_instance_extensions = vk::enumerateInstanceExtensionProperties();

#ifdef USE_VALIDATION_LAYERS
        // Check if VK_EXT_debug_utils is supported, which supersedes VK_EXT_Debug_Report
        bool has_debug_utils = enable_extension(VK_EXT_DEBUG_UTILS_EXTENSION_NAME, available_instance_extensions, enabled_extensions);
        bool has_debug_report = false;

        if (!has_debug_utils)
        {
            has_debug_report = enable_extension(VK_EXT_DEBUG_REPORT_EXTENSION_NAME, available_instance_extensions, enabled_extensions);
            if (!has_debug_report)
            {
                throw std::runtime_error("Neither of VK_EXT_debug_util or VK_EXT_debug_report are available.");
            }
        }
#endif

#ifdef USE_VALIDATION_LAYER_FEATURES
        auto available_layer_instance_extensions = vk::enumerateInstanceExtensionProperties(std::string("VK_LAYER_KHRONOS_validation"));
        enable_extension(VK_EXT_VALIDATION_FEATURES_EXTENSION_NAME, available_layer_instance_extensions, enabled_extensions);
#endif

        // Specific surface extensions are obtained from  Window::get_required_surface_extensions
        // They are already added to requested_extensions by VulkanSample::prepare

        // Even for a headless surface a swapchain is still required
        enable_extension(VK_KHR_SURFACE_EXTENSION_NAME, available_instance_extensions, enabled_extensions);

        // VK_KHR_get_physical_device_properties2 is a prerequisite of VK_KHR_performance_query
        // which will be used for stats gathering where available.
        enable_extension(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME, available_instance_extensions, enabled_extensions);

        for (auto requested_extension : requested_extensions)
        {
            if (!enable_extension(requested_extension, available_instance_extensions, enabled_extensions))
            {
                throw std::runtime_error("Required instance exntensions are missing.");
            }
        }

        auto supported_layers = vk::enumerateInstanceLayerProperties();
        std::vector<const char*> enabled_layers;

        for (auto requested_layer : requested_layers)
        {
            if (!enable_layer(requested_layer, supported_layers, enabled_layers))
            {
                throw std::runtime_error("Required layers are missing.");
            }
        }

#ifdef USE_VALIDATION_LAYERS
        // NOTE: It's important to have the validation layer as the last one here!!!!
        //             Otherwise, device creation fails !?!
        enable_layer("VK_LAYER_KHRONOS_validation", supported_layers, enabled_layers);
#endif

        vk::ApplicationInfo app_info{ application_name.c_str(), {}, "Vulkan Samples", {}, api_version };

        vk::InstanceCreateInfo instance_info{
            {},
            &app_info,
            static_cast<uint32_t>(enabled_layers.size()),
            enabled_layers.data(),
            static_cast<uint32_t>(enabled_extensions.size()),
            enabled_extensions.data()
        };

#ifdef USE_VALIDATION_LAYERS
        vk::DebugUtilsMessengerCreateInfoEXT debug_utils_create_info;
        vk::DebugReportCallbackCreateInfoEXT debug_report_create_info;

        if (has_debug_utils)
        {
            debug_utils_create_info = vk::DebugUtilsMessengerCreateInfoEXT{
                {},
                vk::DebugUtilsMessageSeverityFlagBitsEXT::eError | vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning,
                vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation | vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance,
                debug_utils_messenger_callback
            };
            instance_info.pNext = &debug_utils_create_info;
        }
        else if (has_debug_report)
        {
            debug_report_create_info = vk::DebugReportCallbackCreateInfoEXT{
                vk::DebugReportFlagBitsEXT::eError | vk::DebugReportFlagBitsEXT::eWarning | vk::DebugReportFlagBitsEXT::ePerformanceWarning,
                debug_callback
            };
            instance_info.pNext = &debug_report_create_info;
        }
#endif

        vk::LayerSettingsCreateInfoEXT layerSettingsCreateInfo;

        // If layer settings are defined, then activate the sample's required layer settings during instance creation
        if (required_layer_settings.size() > 0)
        {
            layerSettingsCreateInfo.settingCount = static_cast<uint32_t>(required_layer_settings.size());
            layerSettingsCreateInfo.pSettings    = required_layer_settings.data();
            layerSettingsCreateInfo.pNext        = instance_info.pNext;
            instance_info.pNext                  = &layerSettingsCreateInfo;
        }

        // Create the Vulkan instance
        handle = vk::createInstance(instance_info);

        // initialize the Vulkan-Hpp default dispatcher on the instance
        VULKAN_HPP_DEFAULT_DISPATCHER.init(handle);

        // Need to load volk for all the not-yet Vulkan-Hpp calls
        volkLoadInstance(handle);

#ifdef USE_VALIDATION_LAYERS
        if (has_debug_utils)
        {
            debug_utils_messenger = handle.createDebugUtilsMessengerEXT(debug_utils_create_info);
        }
        else if (has_debug_report)
        {
            debug_report_callback = handle.createDebugReportCallbackEXT(debug_report_create_info);
        }
#endif
    }

    HPPInstance::HPPInstance(vk::Instance instance) :
        handle{instance}
    {
    }

    HPPInstance::~HPPInstance()
    {
#ifdef USE_VALIDATION_LAYERS
        if (debug_utils_messenger)
        {
            handle.destroyDebugUtilsMessengerEXT(debug_utils_messenger);
        }
        if (debug_report_callback)
        {
            handle.destroyDebugReportCallbackEXT(debug_report_callback);
        }
#endif

        if (handle)
        {
            handle.destroy();
        }
    }

    const std::vector<const char*>& HPPInstance::get_extensions()
    {
        return enabled_extensions;
    }

    vk::Instance HPPInstance::get_handle() const
    {
        return handle;
    }

    bool HPPInstance::is_enabled(const char* extension) const
    {
        return std::ranges::find_if(enabled_extensions,
                                    [extension](const char* enabled_extension) {return strcmp(extension, enabled_extension) == 0; }) != enabled_extensions.end();
    }
}
