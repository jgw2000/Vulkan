#pragma once

namespace vkb::rendering
{
    class HPPRenderFrame;
}

namespace vkb::core
{
    class HPPCommandBuffer;

    class HPPCommandPool
    {
    public:
        HPPCommandPool(HPPDevice&                      device,
                       uint32_t                        queue_family_index,
                       vkb::rendering::HPPRenderFrame* render_frame = nullptr,
                       size_t                          thread_index = 0,
                       vkb::CommandBufferResetMode     reset_mode   = vkb::CommandBufferResetMode::ResetPool);
        ~HPPCommandPool();

        HPPCommandPool(const HPPCommandPool&) = delete;
        HPPCommandPool(HPPCommandPool&&) = default;
        
        HPPCommandPool& operator=(const HPPCommandPool&) = delete;
        HPPCommandPool& operator=(HPPCommandPool&&) = delete;

        HPPDevice&                      get_device()                   { return device; }
        vk::CommandPool                 get_handle() const             { return handle; }
        uint32_t                        get_queue_family_index() const { return queue_family_index; }
        vkb::rendering::HPPRenderFrame* get_render_frame()             { return render_frame; }
        vkb::CommandBufferResetMode     get_reset_mode() const         { return reset_mode; }
        size_t                          get_thread_index() const       { return thread_index; }
        void                            reset_pool();

        HPPCommandBuffer& request_command_buffer(vk::CommandBufferLevel level = vk::CommandBufferLevel::ePrimary);

    private:
        HPPDevice&                      device;
        vk::CommandPool                 handle                                = nullptr;
        vkb::rendering::HPPRenderFrame* render_frame                          = nullptr;
        size_t                          thread_index                          = 0;
        uint32_t                        queue_family_index                    = 0;
        std::vector<HPPCommandBuffer>   primary_command_buffers;
        uint32_t                        active_primary_command_buffer_count   = 0;
        std::vector<HPPCommandBuffer>   secondary_command_buffers;
        uint32_t                        active_secondary_command_buffer_count = 0;
        vkb::CommandBufferResetMode     reset_mode                            = vkb::CommandBufferResetMode::ResetPool;
    };
}