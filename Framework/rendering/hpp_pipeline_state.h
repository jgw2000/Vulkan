#pragma once

namespace vkb::rendering
{
    struct HPPVertexInputState
    {
        std::vector<vk::VertexInputBindingDescription>   bindings;
        std::vector<vk::VertexInputAttributeDescription> attributes;
    };

    struct HPPInputAssemblyState
    {
        vk::PrimitiveTopology topology                 = vk::PrimitiveTopology::eTriangleList;
        vk::Bool32            primitive_restart_enable = false;
    };

    struct HPPViewportState
    {
        uint32_t viewport_count = 1;
        uint32_t scissor_count  = 1;
    };

    struct HPPRasterizationState
    {
        vk::Bool32        depth_clamp_enable        = false;
        vk::Bool32        rasterizer_discard_enable = false;
        vk::PolygonMode   polygon_mode              = vk::PolygonMode::eFill;
        vk::CullModeFlags cull_mode                 = vk::CullModeFlagBits::eBack;
        vk::FrontFace     front_face                = vk::FrontFace::eCounterClockwise;
        vk::Bool32        depth_bias_enable         = false;
    };

    struct HPPMultisampleState
    {
        vk::SampleCountFlagBits rasterization_samples    = vk::SampleCountFlagBits::e1;
        vk::Bool32              sample_shading_enable    = false;
        float                   min_sample_shading       = 0.0f;
        vk::SampleMask          sample_mask              = 0;
        vk::Bool32              alpha_to_coverage_enable = false;
        vk::Bool32              alpha_to_one_enable      = false;
    };

    struct HPPStencilOpState
    {
        vk::StencilOp fail_op       = vk::StencilOp::eReplace;
        vk::StencilOp pass_op       = vk::StencilOp::eReplace;
        vk::StencilOp depth_fail_op = vk::StencilOp::eReplace;
        vk::CompareOp compare_op    = vk::CompareOp::eNever;
    };

    struct HPPDepthStencilState
    {
        vk::Bool32        depth_test_enable = true;
        vk::Bool32        depth_write_enable = true;
        vk::CompareOp     depth_compare_op = vk::CompareOp::eGreater;   // Note: Using reversed depth-buffer for increased precision, so Greater depth values are kept
        vk::Bool32        depth_bounds_test_enable = false;
        vk::Bool32        stencil_test_enable = false;
        
        HPPStencilOpState front;
        HPPStencilOpState back;
    };

    struct HPPColorBlendAttachmentState
    {
        vk::Bool32              blend_enable           = false;
        vk::BlendFactor         src_color_blend_factor = vk::BlendFactor::eOne;
        vk::BlendFactor         dst_color_blend_factor = vk::BlendFactor::eZero;
        vk::BlendOp             color_blend_op         = vk::BlendOp::eAdd;
        vk::BlendFactor         src_alpha_blend_factor = vk::BlendFactor::eOne;
        vk::BlendFactor         dst_alpha_blend_factor = vk::BlendFactor::eZero;
        vk::BlendOp             alpha_blend_op         = vk::BlendOp::eAdd;
        vk::ColorComponentFlags color_write_mask       = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA;
    };

    struct HPPColorBlendState
    {
        vk::Bool32                                logic_op_enable = false;
        vk::LogicOp                               logic_op        = vk::LogicOp::eClear;
        std::vector<HPPColorBlendAttachmentState> attachments;
    };
}