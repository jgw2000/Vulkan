#include "stdafx.h"
#include "vulkan_resource.h"

namespace vkb::core
{
    template <typename Handle>
    VulkanResource<Handle>::VulkanResource(VulkanResource&& other) :
        handle(std::exchange(other.handle, {})),
        device(std::exchange(other.device, {}))
    {}

    template <typename Handle>
    VulkanResource<Handle>& VulkanResource<Handle>::operator=(VulkanResource&& other)
    {
        handle = std::exchange(other.handle, {});
        device = std::exchange(other.device, {});
        return *this;
    }
}
