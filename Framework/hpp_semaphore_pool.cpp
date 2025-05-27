#include "stdafx.h"

namespace vkb
{
    HPPSemaphorePool::HPPSemaphorePool(core::HPPDevice& device) :
        device{device}
    { }

    HPPSemaphorePool::~HPPSemaphorePool()
    {
        reset();

        // Destroy all semaphores
        for (auto semaphore : semaphores)
        {
            device.get_handle().destroySemaphore(semaphore);
        }

        semaphores.clear();
    }

    vk::Semaphore HPPSemaphorePool::request_semaphore()
    {
        // Check if there is an available semaphore
        if (active_semaphore_count < semaphores.size())
        {
            return semaphores[active_semaphore_count++];
        }

        vk::SemaphoreCreateInfo create_info{};
        vk::Semaphore semaphore = device.get_handle().createSemaphore(create_info);

        semaphores.push_back(semaphore);
        active_semaphore_count++;

        return semaphore;
    }

    vk::Semaphore HPPSemaphorePool::request_semaphore_with_ownership()
    {
        // Check if there is an available semaphore
        if (active_semaphore_count < semaphores.size())
        {
            vk::Semaphore semaphore = semaphores.back();
            semaphores.pop_back();
            return semaphore;
        }

        // Otherwise, we need to create one, and don't keep track of it, app will release
        vk::SemaphoreCreateInfo create_info{};
        vk::Semaphore semaphore = device.get_handle().createSemaphore(create_info);

        return semaphore;
    }

    void HPPSemaphorePool::release_owned_semaphore(vk::Semaphore semaphore)
    {
        // We cannot reuse this semaphore until ::reset()
        released_semaphores.push_back(semaphore);
    }

    void HPPSemaphorePool::reset()
    {
        active_semaphore_count = 0;

        // Now we can safely recycle the released semaphores
        for (auto& sem : released_semaphores)
        {
            semaphores.push_back(sem);
        }

        released_semaphores.clear();
    }
}