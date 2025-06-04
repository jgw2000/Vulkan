#include "stdafx.h"

bool operator==(const vk::VertexInputBindingDescription& lhs, const vk::VertexInputBindingDescription& rhs)
{
    return std::tie(lhs.binding, lhs.inputRate, lhs.stride) == std::tie(rhs.binding, rhs.inputRate, rhs.stride);
}

bool operator==(const vk::VertexInputAttributeDescription& lhs, const vk::VertexInputAttributeDescription& rhs)
{
    return std::tie(lhs.binding, lhs.format, lhs.location, lhs.offset) == std::tie(rhs.binding, rhs.format, rhs.location, rhs.offset);
}

bool operator==(const vkb::rendering::HPPColorBlendAttachmentState& lhs, const vkb::rendering::HPPColorBlendAttachmentState& rhs)
{
    return std::tie(lhs.alpha_blend_op, lhs.blend_enable, lhs.color_blend_op, lhs.color_write_mask, lhs.dst_alpha_blend_factor, lhs.dst_color_blend_factor, lhs.src_alpha_blend_factor, lhs.src_color_blend_factor) ==
           std::tie(rhs.alpha_blend_op, rhs.blend_enable, rhs.color_blend_op, rhs.color_write_mask, rhs.dst_alpha_blend_factor, rhs.dst_color_blend_factor, rhs.src_alpha_blend_factor, rhs.src_color_blend_factor);
}

bool operator!=(const vkb::rendering::HPPStencilOpState& lhs, const vkb::rendering::HPPStencilOpState& rhs)
{
    return std::tie(lhs.compare_op, lhs.depth_fail_op, lhs.fail_op, lhs.pass_op) != std::tie(rhs.compare_op, rhs.depth_fail_op, rhs.fail_op, rhs.pass_op);
}

bool operator!=(const vkb::rendering::HPPVertexInputState& lhs, const vkb::rendering::HPPVertexInputState& rhs)
{
    return lhs.bindings != rhs.bindings || lhs.attributes != rhs.attributes;
}

bool operator!=(const vkb::rendering::HPPInputAssemblyState& lhs, const vkb::rendering::HPPInputAssemblyState& rhs)
{
    return std::tie(lhs.primitive_restart_enable, lhs.topology) != std::tie(rhs.primitive_restart_enable, rhs.topology);
}

bool operator!=(const vkb::rendering::HPPViewportState& lhs, const vkb::rendering::HPPViewportState& rhs)
{
    return lhs.viewport_count != rhs.viewport_count || lhs.scissor_count != rhs.scissor_count;
}

bool operator!=(const vkb::rendering::HPPRasterizationState& lhs, const vkb::rendering::HPPRasterizationState& rhs)
{
    return std::tie(lhs.cull_mode, lhs.depth_bias_enable, lhs.depth_clamp_enable, lhs.front_face, lhs.polygon_mode, lhs.rasterizer_discard_enable) !=
           std::tie(rhs.cull_mode, rhs.depth_bias_enable, rhs.depth_clamp_enable, rhs.front_face, rhs.polygon_mode, rhs.rasterizer_discard_enable);
}

bool operator!=(const vkb::rendering::HPPMultisampleState& lhs, const vkb::rendering::HPPMultisampleState& rhs)
{
    return std::tie(lhs.alpha_to_coverage_enable, lhs.alpha_to_one_enable, lhs.min_sample_shading, lhs.rasterization_samples, lhs.sample_mask, lhs.sample_shading_enable) !=
           std::tie(rhs.alpha_to_coverage_enable, rhs.alpha_to_one_enable, rhs.min_sample_shading, rhs.rasterization_samples, rhs.sample_mask, rhs.sample_shading_enable);
}

bool operator!=(const vkb::rendering::HPPDepthStencilState& lhs, const vkb::rendering::HPPDepthStencilState& rhs)
{
    return std::tie(lhs.depth_bounds_test_enable, lhs.depth_compare_op, lhs.depth_test_enable, lhs.depth_write_enable, lhs.stencil_test_enable) !=
        std::tie(rhs.depth_bounds_test_enable, rhs.depth_compare_op, rhs.depth_test_enable, rhs.depth_write_enable, rhs.stencil_test_enable) ||
        lhs.back != rhs.back || lhs.front != rhs.front;
}

bool operator!=(const vkb::rendering::HPPColorBlendState& lhs, const vkb::rendering::HPPColorBlendState& rhs)
{
    return std::tie(lhs.logic_op, lhs.logic_op_enable) != std::tie(rhs.logic_op, rhs.logic_op_enable) ||
           lhs.attachments.size() != rhs.attachments.size() ||
           !std::equal(lhs.attachments.begin(), lhs.attachments.end(), rhs.attachments.begin(),
                      [](const vkb::rendering::HPPColorBlendAttachmentState& lhs, const vkb::rendering::HPPColorBlendAttachmentState& rhs) {
                          return lhs == rhs;
                      });
}

namespace vkb::rendering
{
    void HPPPipelineState::reset()
    {
        clear_dirty();

        pipeline_layout      = nullptr;
        render_pass          = nullptr;
        vertex_input_state   = {};
        input_assembly_state = {};
        viewport_state       = {};
        rasterization_state  = {};
        multisample_state    = {};
        depth_stencil_state  = {};
        color_blend_state    = {};
        subpass_index        = { 0U };
    }

    void HPPPipelineState::set_pipeline_layout(vkb::core::HPPPipelineLayout& new_pipeline_layout)
    {
        if (pipeline_layout)
        {
            if (pipeline_layout->get_handle() != new_pipeline_layout.get_handle())
            {
                pipeline_layout = &new_pipeline_layout;
                dirty = true;
            }
        }
        else
        {
            pipeline_layout = &new_pipeline_layout;
            dirty = true;
        }
    }

    void HPPPipelineState::set_render_pass(const vkb::core::HPPRenderPass& new_render_pass)
    {
        if (render_pass)
        {
            if (render_pass->get_handle() != new_render_pass.get_handle())
            {
                render_pass = &new_render_pass;
                dirty = true;
            }
        }
        else
        {
            render_pass = &new_render_pass;
            dirty = true;
        }
    }

    void HPPPipelineState::set_vertex_input_state(const HPPVertexInputState& new_vertex_input_state)
    {
        if (vertex_input_state != new_vertex_input_state)
        {
            vertex_input_state = new_vertex_input_state;
            dirty = true;
        }
    }

    void HPPPipelineState::set_input_assembly_state(const HPPInputAssemblyState& new_input_assembly_state)
    {
        if (input_assembly_state != new_input_assembly_state)
        {
            input_assembly_state = new_input_assembly_state;
            dirty = true;
        }
    }

    void HPPPipelineState::set_viewport_state(const HPPViewportState& new_viewport_state)
    {
        if (viewport_state != new_viewport_state)
        {
            viewport_state = new_viewport_state;
            dirty = true;
        }
    }

    void HPPPipelineState::set_rasterization_state(const HPPRasterizationState& new_rasterization_state)
    {
        if (rasterization_state != new_rasterization_state)
        {
            rasterization_state = new_rasterization_state;
            dirty = true;
        }
    }

    void HPPPipelineState::set_multisample_state(const HPPMultisampleState& new_multisample_state)
    {
        if (multisample_state != new_multisample_state)
        {
            multisample_state = new_multisample_state;
            dirty = true;
        }
    }

    void HPPPipelineState::set_depth_stencil_state(const HPPDepthStencilState& new_depth_stencil_state)
    {
        if (depth_stencil_state != new_depth_stencil_state)
        {
            depth_stencil_state = new_depth_stencil_state;
            dirty = true;
        }
    }

    void HPPPipelineState::set_color_blend_state(const HPPColorBlendState& new_color_blend_state)
    {
        if (color_blend_state != new_color_blend_state)
        {
            color_blend_state = new_color_blend_state;
            dirty = true;
        }
    }

    void HPPPipelineState::set_subpass_index(uint32_t new_subpass_index)
    {
        if (subpass_index != new_subpass_index)
        {
            subpass_index = new_subpass_index;
            dirty = true;
        }
    }
}