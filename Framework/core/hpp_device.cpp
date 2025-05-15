#include "stdafx.h"
#include "hpp_device.h"
#include "hpp_physical_device.h"

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
            }
        }
    }
}
