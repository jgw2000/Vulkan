#include "stdafx.h"

namespace vkb::core
{
    HPPDevice::HPPDevice(HPPPhysicalDevice&       gpu,
                         vk::SurfaceKHR           surface,
                         std::vector<const char*> requested_extensions) :
        VulkanResource{ nullptr, this },
        gpu{ gpu }
    {
        // Prepare the device queues
        std::vector<vk::QueueFamilyProperties> queue_family_properties = gpu.get_queue_family_properties();
        std::vector<vk::DeviceQueueCreateInfo> queue_create_infos(queue_family_properties.size());
        std::vector<std::vector<float>>        queue_priorities(queue_family_properties.size());

        for (uint32_t queue_family_index = 0U; queue_family_index < queue_family_properties.size(); ++queue_family_index)
        {
            const auto& queue_family_property = queue_family_properties[queue_family_index];

            if (gpu.has_high_priority_graphics_queue())
            {
                uint32_t graphics_queue_family = get_queue_family_index(vk::QueueFlagBits::eGraphics);
                if (graphics_queue_family == queue_family_index)
                {
                    queue_priorities[queue_family_index].reserve(queue_family_property.queueCount);
                    queue_priorities[queue_family_index].push_back(1.0f);
                    for (uint32_t i = 1; i < queue_family_property.queueCount; ++i)
                    {
                        queue_priorities[queue_family_index].push_back(0.5f);
                    }
                }
                else
                {
                    queue_priorities[queue_family_index].resize(queue_family_property.queueCount, 0.5f);
                }
            }
            else
            {
                queue_priorities[queue_family_index].resize(queue_family_property.queueCount, 0.5f);
            }

            vk::DeviceQueueCreateInfo& queue_create_info = queue_create_infos[queue_family_index];

            queue_create_info.queueFamilyIndex = queue_family_index;
            queue_create_info.queueCount = queue_family_property.queueCount;
            queue_create_info.pQueuePriorities = queue_priorities[queue_family_index].data();
        }

        // Check extensions to enable Vma Dedicated Allocation
        bool can_get_memory_requirements = is_extension_supported(VK_KHR_GET_MEMORY_REQUIREMENTS_2_EXTENSION_NAME);
        bool has_dedicated_allocation = is_extension_supported(VK_KHR_DEDICATED_ALLOCATION_EXTENSION_NAME);

        if (can_get_memory_requirements && has_dedicated_allocation)
        {
            enabled_extensions.push_back(VK_KHR_GET_MEMORY_REQUIREMENTS_2_EXTENSION_NAME);
            enabled_extensions.push_back(VK_KHR_DEDICATED_ALLOCATION_EXTENSION_NAME);
        }

        // For performance queries, we also use host query reset since queryPool resets cannot
        // live in the same command buffer as beginQuery
        if (is_extension_supported(VK_KHR_PERFORMANCE_QUERY_EXTENSION_NAME) && is_extension_supported(VK_EXT_HOST_QUERY_RESET_EXTENSION_NAME))
        {
            auto perf_counter_features = gpu.get_extension_features<vk::PhysicalDevicePerformanceQueryFeaturesKHR>();
            auto host_query_reset_features = gpu.get_extension_features<vk::PhysicalDeviceHostQueryResetFeatures>();

            if (perf_counter_features.performanceCounterQueryPools && host_query_reset_features.hostQueryReset)
            {
                gpu.add_extension_features<vk::PhysicalDevicePerformanceQueryFeaturesKHR>().performanceCounterQueryPools = true;
                gpu.add_extension_features<vk::PhysicalDeviceHostQueryResetFeatures>().hostQueryReset = true;
                enabled_extensions.push_back(VK_KHR_PERFORMANCE_QUERY_EXTENSION_NAME);
                enabled_extensions.push_back(VK_EXT_HOST_QUERY_RESET_EXTENSION_NAME);
            }
        }

        // Check that extensions are supported before trying to create the device
        for (auto& extension : requested_extensions)
        {
            if (is_extension_supported(extension))
            {
                enabled_extensions.emplace_back(extension);
            }
            else
            {
                throw std::runtime_error("Device extension not present");
            }
        }

        // Create the device
        vk::DeviceCreateInfo create_info{
            {},
            static_cast<uint32_t>(queue_create_infos.size()),
            queue_create_infos.data(),
            {},
            {},
            static_cast<uint32_t>(enabled_extensions.size()),
            enabled_extensions.data(),
            &gpu.get_mutable_requested_features()
        };

        // Latest requested feature will have the pNext's all set up for device creation.
        create_info.pNext = gpu.get_extension_feature_chain();

        set_handle(gpu.get_handle().createDevice(create_info));

        queues.resize(queue_family_properties.size());

        for (uint32_t queue_family_index = 0U; queue_family_index < queue_family_properties.size(); ++queue_family_index)
        {
            const auto& queue_family_property = queue_family_properties[queue_family_index];
            auto present_supported = gpu.get_handle().getSurfaceSupportKHR(queue_family_index, surface);

            for (uint32_t queue_index = 0U; queue_index < queue_family_property.queueCount; ++queue_index)
            {
                queues[queue_family_index].emplace_back(*this, queue_family_index, queue_family_property, present_supported, queue_index);
            }
        }

        VULKAN_HPP_DEFAULT_DISPATCHER.init(get_handle());

        vkb::allocated::init(*this);

        // TODO
    }

    HPPDevice::~HPPDevice()
    {
        vkb::allocated::shutdown();

        if (get_handle())
        {
            get_handle().destroy();
        }
    }

    const HPPQueue& HPPDevice::get_queue(uint32_t queue_family_index, uint32_t queue_index) const
    {
        return queues[queue_family_index][queue_index];
    }

    const HPPQueue& HPPDevice::get_queue_by_flags(vk::QueueFlags required_queue_flags, uint32_t queue_index) const
    {
        for (size_t queue_family_index = 0U; queue_family_index < queues.size(); ++queue_family_index)
        {
            const auto& first_queue = queues[queue_family_index][0];

            vk::QueueFlags queue_flags = first_queue.get_properties().queueFlags;
            uint32_t       queue_count = first_queue.get_properties().queueCount;

            if (((queue_flags & required_queue_flags) == required_queue_flags) && queue_index < queue_count)
            {
                return queues[queue_family_index][queue_index];
            }
        }

        throw std::runtime_error("Queue not found");
    }

    const HPPQueue& HPPDevice::get_queue_by_present(uint32_t queue_index) const
    {
        for (size_t queue_family_index = 0U; queue_family_index < queues.size(); ++queue_family_index)
        {
            const auto& first_queue = queues[queue_family_index][0];
            uint32_t queue_count = first_queue.get_properties().queueCount;

            if (first_queue.support_present() && queue_index < queue_count)
            {
                return queues[queue_family_index][queue_index];
            }
        }

        throw std::runtime_error("Queue not found");
    }

    const HPPQueue& HPPDevice::get_suitable_graphics_queue() const
    {
        for (size_t queue_family_index = 0U; queue_family_index < queues.size(); ++queue_family_index)
        {
            const auto& first_queue = queues[queue_family_index][0];
            uint32_t queue_count = first_queue.get_properties().queueCount;

            if (first_queue.support_present() && 0 < queue_count)
            {
                return queues[queue_family_index][0];
            }
        }

        return get_queue_by_flags(vk::QueueFlagBits::eGraphics, 0);
    }

    bool HPPDevice::is_extension_supported(const std::string& requested_extension) const
    {
        return gpu.is_extension_supported(requested_extension);
    }

    bool HPPDevice::is_enabled(const std::string& extension) const
    {
        return std::ranges::find_if(enabled_extensions,
            [extension](const char* enabled_extension) { return extension == enabled_extension; }) != enabled_extensions.end();
    }

    uint32_t HPPDevice::get_queue_family_index(vk::QueueFlagBits queue_flag) const
    {
        const auto& queue_family_properties = gpu.get_queue_family_properties();

        // Dedicated queue for compute
        // Try to find a queue family index that supports compute but not graphics
        if (queue_flag & vk::QueueFlagBits::eCompute)
        {
            for (uint32_t i = 0; i < static_cast<uint32_t>(queue_family_properties.size()); ++i)
            {
                if ((queue_family_properties[i].queueFlags & queue_flag) && !(queue_family_properties[i].queueFlags & vk::QueueFlagBits::eGraphics))
                {
                    return i;
                    break;
                }
            }
        }

        // Dedicated queue for transfer
        // Try to find a queue family index that supports transfer but not graphics and compute
        if (queue_flag & vk::QueueFlagBits::eTransfer)
        {
            for (uint32_t i = 0; i < static_cast<uint32_t>(queue_family_properties.size()); ++i)
            {
                if ((queue_family_properties[i].queueFlags & queue_flag) && !(queue_family_properties[i].queueFlags & vk::QueueFlagBits::eGraphics) &&
                    !(queue_family_properties[i].queueFlags & vk::QueueFlagBits::eCompute))
                {
                    return i;
                    break;
                }
            }
        }

        // For other queue types or if no separate compute queue is present, return the first one to support the requested flags
        for (uint32_t i = 0; i < static_cast<uint32_t>(queue_family_properties.size()); ++i)
        {
            if (queue_family_properties[i].queueFlags & queue_flag)
            {
                return i;
                break;
            }
        }

        throw std::runtime_error("Could not find a matching queue family index");
    }
}
