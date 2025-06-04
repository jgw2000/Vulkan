#pragma once

namespace vkb::core
{
    class HPPPipelineLayout;
}

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

    class HPPPipelineState
    {
    public:
        const vkb::core::HPPPipelineLayout& get_pipeline_layout() const      { return *pipeline_layout; }
        const vkb::core::HPPRenderPass*     get_render_pass() const          { return render_pass; }
        const HPPVertexInputState&          get_vertex_input_state() const   { return vertex_input_state; }
        const HPPInputAssemblyState&        get_input_assembly_state() const { return input_assembly_state; }
        const HPPViewportState&             get_viewport_state() const       { return viewport_state; }
        const HPPRasterizationState&        get_rasterization_state() const  { return rasterization_state; }
        const HPPMultisampleState&          get_multisample_state() const    { return multisample_state; }
        const HPPDepthStencilState&         get_depth_stencil_state() const  { return depth_stencil_state; }
        const HPPColorBlendState&           get_color_blend_state() const    { return color_blend_state; }
        uint32_t                            get_subpass_index() const        { return subpass_index; }
        bool                                is_dirty() const                 { return dirty; /* TODO */ }
        void                                clear_dirty()                    { dirty = false; /* TODO */ }

        void reset();
        void set_pipeline_layout(vkb::core::HPPPipelineLayout& pipeline_layout);
        void set_render_pass(const vkb::core::HPPRenderPass& render_pass);
        void set_vertex_input_state(const HPPVertexInputState& vertex_input_state);
        void set_input_assembly_state(const HPPInputAssemblyState& input_assembly_state);
        void set_viewport_state(const HPPViewportState& viewport_state);
        void set_rasterization_state(const HPPRasterizationState& rasterization_state);
        void set_multisample_state(const HPPMultisampleState& multisample_state);
        void set_depth_stencil_state(const HPPDepthStencilState& depth_stencil_state);
        void set_color_blend_state(const HPPColorBlendState& color_blend_state);
        void set_subpass_index(uint32_t subpass_index);

    private:
        bool dirty{ false };

        vkb::core::HPPPipelineLayout* pipeline_layout{ nullptr };

        const vkb::core::HPPRenderPass* render_pass{ nullptr };

        HPPVertexInputState vertex_input_state{};

        HPPInputAssemblyState input_assembly_state{};

        HPPViewportState viewport_state{};

        HPPRasterizationState rasterization_state{};

        HPPMultisampleState multisample_state{};

        HPPDepthStencilState depth_stencil_state{};

        HPPColorBlendState color_blend_state{};

        uint32_t subpass_index{ 0 };
    };
}