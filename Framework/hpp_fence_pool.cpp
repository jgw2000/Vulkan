#include "stdafx.h"

namespace vkb
{
    HPPFencePool::HPPFencePool(core::HPPDevice& device) :
        device{device}
    { }

    HPPFencePool::~HPPFencePool()
    {
        wait();
        reset();

        // Destroy all fences
        for (auto fence : fences)
        {
            device.get_handle().destroyFence(fence);
        }

        fences.clear();
    }

    vk::Fence HPPFencePool::request_fence()
    {
        // Check if there is an available fnece
        if (active_fence_count < fences.size())
        {
            return fences[active_fence_count];
        }

        vk::FenceCreateInfo create_info{};
        vk::Fence fence = device.get_handle().createFence(create_info);

        fences.push_back(fence);
        active_fence_count++;

        return fence;
    }

    vk::Result HPPFencePool::wait(uint64_t timeout) const
    {
        if (active_fence_count < 1 || fences.empty())
        {
            return vk::Result::eSuccess;
        }

        return device.get_handle().waitForFences(active_fence_count, fences.data(), true, timeout);
    }

    vk::Result HPPFencePool::reset()
    {
        if (active_fence_count < 1 || fences.empty())
        {
            return vk::Result::eSuccess;
        }

        vk::Result result = device.get_handle().resetFences(active_fence_count, fences.data());
        if (result != vk::Result::eSuccess)
        {
            return result;
        }

        active_fence_count = 0;

        return vk::Result::eSuccess;
    }
}