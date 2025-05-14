#include "stdafx.h"
#include "hpp_physical_device.h"

namespace vkb::core
{
    HPPPhysicalDevice::HPPPhysicalDevice(HPPInstance& instance, vk::PhysicalDevice physical_device) :
        instance{ instance },
        handle{ physical_device }
    {
        features                = physical_device.getFeatures();
        properties              = physical_device.getProperties();
        memory_properties       = physical_device.getMemoryProperties();
        queue_family_properties = physical_device.getQueueFamilyProperties();
        device_extensions       = physical_device.enumerateDeviceExtensionProperties();
    }

    void* HPPPhysicalDevice::get_extension_feature_chain() const
    {
        return last_requested_extension_feature;
    }

    bool HPPPhysicalDevice::is_extension_supported(const std::string& requested_extension) const
    {
        return std::ranges::find_if(device_extensions,
                                    [requested_extension](auto& device_extension) { return std::strcmp(device_extension.extensionName, requested_extension.c_str()) == 0; }) != device_extensions.end();
    }

    const vk::PhysicalDeviceFeatures& HPPPhysicalDevice::get_features() const
    {
        return features;
    }

    vk::PhysicalDevice HPPPhysicalDevice::get_handle() const
    {
        return handle;
    }

    HPPInstance& HPPPhysicalDevice::get_instance() const
    {
        return instance;
    }

    const vk::PhysicalDeviceMemoryProperties& HPPPhysicalDevice::get_memory_properties() const
    {
        return memory_properties;
    }

    const vk::PhysicalDeviceProperties& HPPPhysicalDevice::get_properties() const
    {
        return properties;
    }

    const std::vector<vk::QueueFamilyProperties>& HPPPhysicalDevice::get_queue_family_properties() const
    {
        return queue_family_properties;
    }

    const vk::PhysicalDeviceFeatures HPPPhysicalDevice::get_requested_features() const
    {
        return requested_features;
    }

    vk::PhysicalDeviceFeatures& HPPPhysicalDevice::get_mutable_requested_features()
    {
        return requested_features;
    }
}
