#include "stdafx.h"

namespace vkb::core
{
    HPPQueue::HPPQueue(HPPDevice& device, uint32_t family_index, vk::QueueFamilyProperties properties, vk::Bool32 can_present, uint32_t index) :
        device{ device },
        family_index{ family_index },
        index{ index },
        can_present{ can_present },
        properties{ properties }
    {
        handle = device.get_handle().getQueue(family_index, index);
    }

    HPPQueue::HPPQueue(HPPQueue&& other) :
        device(other.device),
        handle(std::exchange(other.handle, {})),
        family_index(std::exchange(other.family_index, {})),
        index(std::exchange(other.index, 0)),
        can_present(std::exchange(other.can_present, false)),
        properties(std::exchange(other.properties, {}))
    {
    }
}
