#pragma once

namespace vkb
{
    class HPPFencePool
    {
    public:
        HPPFencePool(core::HPPDevice& device);
        ~HPPFencePool();

        HPPFencePool(const HPPFencePool&) = delete;
        HPPFencePool(HPPFencePool&&) = delete;

        HPPFencePool& operator=(const HPPFencePool&) = delete;
        HPPFencePool& operator=(HPPFencePool&&) = delete;

        vk::Fence  request_fence();
        vk::Result wait(uint64_t timeout = std::numeric_limits<uint64_t>::max()) const;
        vk::Result reset();

    private:
        core::HPPDevice& device;

        std::vector<vk::Fence> fences;

        uint32_t active_fence_count{ 0 };
    };
}