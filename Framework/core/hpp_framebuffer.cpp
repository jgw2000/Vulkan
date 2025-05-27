#include "stdafx.h"

namespace vkb::core
{
    HPPFramebuffer::HPPFramebuffer(HPPDevice& device,
                                   const vkb::rendering::HPPRenderTarget& render_target,
                                   const HPPRenderPass& render_pass) :
        device{ device },
        extent{ render_target.get_extent() }
    {
        std::vector<vk::ImageView> attachments;

        for (auto& view : render_target.get_views())
        {
            attachments.emplace_back(view.get_handle());
        }

        vk::FramebufferCreateInfo create_info{
            {},
            render_pass.get_handle(),
            static_cast<uint32_t>(attachments.size()),
            attachments.data(),
            extent.width,
            extent.height,
            1
        };

        handle = device.get_handle().createFramebuffer(create_info);
    }

    HPPFramebuffer::~HPPFramebuffer()
    {
        if (handle != nullptr)
        {
            device.get_handle().destroyFramebuffer(handle);
        }
    }

    HPPFramebuffer::HPPFramebuffer(HPPFramebuffer&& other) :
        device{ other.device },
        handle{ other.handle },
        extent{ other.extent }
    {
        other.handle = nullptr;
    }
}