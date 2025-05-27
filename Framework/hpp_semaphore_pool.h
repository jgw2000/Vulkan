#pragma once

namespace vkb
{
    class HPPSemaphorePool
    {
    public:
        HPPSemaphorePool(core::HPPDevice& device);
        ~HPPSemaphorePool();

        HPPSemaphorePool(const HPPSemaphorePool&) = delete;
        HPPSemaphorePool(HPPSemaphorePool&&) = delete;

        HPPSemaphorePool& operator=(const HPPSemaphorePool&) = delete;
        HPPSemaphorePool& operator=(HPPSemaphorePool&&) = delete;

        vk::Semaphore request_semaphore();
        vk::Semaphore request_semaphore_with_ownership();
        void release_owned_semaphore(vk::Semaphore semaphore);

        void reset();

        uint32_t get_active_semaphore_count() const { return active_semaphore_count; }

    private:
        core::HPPDevice& device;

        std::vector<vk::Semaphore> semaphores;
        std::vector<vk::Semaphore> released_semaphores;

        uint32_t active_semaphore_count{ 0 };
    };
}