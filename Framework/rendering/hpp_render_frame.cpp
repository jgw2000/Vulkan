#include "stdafx.h"

namespace vkb::rendering
{
    HPPRenderFrame::HPPRenderFrame(vkb::core::HPPDevice& device, std::unique_ptr<HPPRenderTarget>&& render_target, size_t thread_count) :
        device{ device },
        fence_pool{device},
        semaphore_pool{device},
        swapchain_render_target{ std::move(render_target) },
        thread_count{ thread_count }
    {
        // TODO
    }

    vk::Fence HPPRenderFrame::request_fence()
    {
        return fence_pool.request_fence();
    }

    vk::Semaphore HPPRenderFrame::request_semaphore()
    {
        return semaphore_pool.request_semaphore();
    }

    vk::Semaphore HPPRenderFrame::request_semaphore_with_ownership()
    {
        return semaphore_pool.request_semaphore_with_ownership();
    }

    void HPPRenderFrame::reset()
    {
        fence_pool.wait();
        fence_pool.reset();

        for (auto& command_pools_per_queue : command_pools)
        {
            for (auto& command_pool : command_pools_per_queue.second)
            {
                command_pool->reset_pool();
            }
        }

        semaphore_pool.reset();

        // TODO
    }

    vkb::core::HPPCommandBuffer& HPPRenderFrame::request_command_buffer(
        const vkb::core::HPPQueue&  queue,
        vkb::CommandBufferResetMode reset_mode,
        vk::CommandBufferLevel      level,
        size_t                      thread_index
    )
    {
        assert(thread_index < thread_count && "Thread index is out of bounds");

        auto& command_pools = get_command_pools(queue, reset_mode);
        auto command_pool_it =
            std::ranges::find_if(command_pools,
                [&thread_index](std::unique_ptr<vkb::core::HPPCommandPool>& cmd_pool) { return cmd_pool->get_thread_index() == thread_index; });
        assert(command_pool_it != command_pools.end());

        return (*command_pool_it)->request_command_buffer(level);
    }

    void HPPRenderFrame::update_render_target(std::unique_ptr<HPPRenderTarget>&& render_target)
    {
        swapchain_render_target = std::move(render_target);
    }

    std::vector<std::unique_ptr<vkb::core::HPPCommandPool>>& HPPRenderFrame::get_command_pools(
        const vkb::core::HPPQueue& queue,
        vkb::CommandBufferResetMode reset_mode
    )
    {
        auto command_pool_it = command_pools.find(queue.get_family_index());

        if (command_pool_it != command_pools.end())
        {
            assert(!command_pool_it->second.empty());
            if (command_pool_it->second[0]->get_reset_mode() != reset_mode)
            {
                device.get_handle().waitIdle();

                // Delete pools
                command_pools.erase(command_pool_it);
            }
            else
            {
                return command_pool_it->second;
            }
        }

        bool inserted = false;
        std::tie(command_pool_it, inserted) = command_pools.emplace(queue.get_family_index(), std::vector<std::unique_ptr<vkb::core::HPPCommandPool>>());
        if (!inserted)
        {
            throw std::runtime_error("Failed to insert command pool");
        }

        for (size_t i = 0; i < thread_count; ++i)
        {
            command_pool_it->second.push_back(std::make_unique<vkb::core::HPPCommandPool>(device, queue.get_family_index(), this, i, reset_mode));
        }

        return command_pool_it->second;
    }
}