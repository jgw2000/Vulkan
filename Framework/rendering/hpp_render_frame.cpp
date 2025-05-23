#include "stdafx.h"

namespace vkb::rendering
{
    HPPRenderFrame::HPPRenderFrame(vkb::core::HPPDevice& device, std::unique_ptr<HPPRenderTarget>&& render_target, size_t thread_count) :
        device{ device },
        swapchain_render_target{ std::move(render_target) },
        thread_count{ thread_count }
    {
        // TODO
    }
}