#pragma once

namespace vkb::rendering
{
    class HPPRenderTarget;
}

namespace vkb::core
{
    class HPPRenderPass;

    class HPPFramebuffer
    {
    public:
        HPPFramebuffer(HPPDevice& device, const vkb::rendering::HPPRenderTarget& render_target, const HPPRenderPass& render_pass);
        ~HPPFramebuffer();

        HPPFramebuffer(const HPPFramebuffer&) = delete;
        HPPFramebuffer(HPPFramebuffer&& other);

        HPPFramebuffer& operator=(const HPPFramebuffer&) = delete;
        HPPFramebuffer& operator=(HPPFramebuffer&&) = delete;

        vk::Framebuffer     get_handle() const { return handle; }
        const vk::Extent2D& get_extent() const { return extent; }

    private:
        HPPDevice& device;

        vk::Framebuffer handle{ nullptr };
        
        vk::Extent2D extent{};
    };
}