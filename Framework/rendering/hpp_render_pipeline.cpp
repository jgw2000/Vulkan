#include "stdafx.h"

namespace vkb::rendering
{
    HPPRenderPipeline::HPPRenderPipeline(std::vector<std::unique_ptr<HPPSubpass>>&& subpasses_) :
        subpasses{ std::move(subpasses_) }
    {
        prepare();

        // Default load/store for swapchain
        load_store[0].load_op  = vk::AttachmentLoadOp::eClear;
        load_store[0].store_op = vk::AttachmentStoreOp::eStore;

        // Default load/store for depth attachment
        load_store[1].load_op  = vk::AttachmentLoadOp::eClear;
        load_store[1].store_op = vk::AttachmentStoreOp::eDontCare;

        // Default clear value
        clear_value[0].color                = { 0.0f, 0.0f, 0.0f, 1.0f };
        clear_value[1].depthStencil.depth   = 0.0f;
        clear_value[1].depthStencil.stencil = ~0U;
    }

    void HPPRenderPipeline::prepare()
    {
        for (auto& subpass : subpasses)
        {
            subpass->prepare();
        }
    }

    void HPPRenderPipeline::add_subpass(std::unique_ptr<HPPSubpass>&& subpass)
    {
        subpass->prepare();
        subpasses.emplace_back(std::move(subpass));
    }

    void HPPRenderPipeline::draw(vkb::core::HPPCommandBuffer& command_buffer, HPPRenderTarget& render_target, vk::SubpassContents contents)
    {
        assert(!subpasses.empty() && "Render pipeline should contain at least one sub-pass");

        vk::ClearValue cv = {};
        cv.color = { 0.0f, 0.0f, 0.0f, 1.0f };

        // Pad clear values if they're less than render target attachements
        while (clear_value.size() < render_target.get_attachments().size())
        {
            clear_value.push_back(cv);
        }

        for (size_t i = 0; i < subpasses.size(); ++i)
        {
            active_subpass_index = i;
            auto& subpass = subpasses[i];
            subpass->update_render_target_attachments(render_target);

            if (i == 0)
            {
                command_buffer.begin_render_pass(render_target, load_store, clear_value, subpasses, contents);
            }
            else
            {
                command_buffer.next_subpass();
            }

            subpass->draw(command_buffer);
        }

        active_subpass_index = 0;
    }
}