#pragma once

namespace vkb::rendering
{
    class HPPSubpass
    {
    public:
        /**
         * @brief Prepares the shaders and shader variants for a subpass
         */
        virtual void prepare() = 0;

        /**
         * @brief Draw virtual function
         * @param command_buffer The command buffer to record the draw commands into
         */
        virtual void draw(vkb::core::HPPCommandBuffer& command_buffer) = 0;

        const std::vector<uint32_t>& get_input_attachments() const                { return input_attachments; }
        const std::vector<uint32_t>& get_output_attachments() const               { return output_attachments; }
        const std::vector<uint32_t>& get_color_resolve_attachments() const        { return color_resolve_attachments; }
        bool                         get_disable_depth_stencil_attachment() const { return disable_depth_stencil_attachment; }
        vk::ResolveModeFlagBits      get_depth_stencil_resolve_mode() const       { return depth_stencil_resolve_mode; }
        uint32_t                     get_depth_stencil_resolve_attachment() const { return depth_stencil_resolve_attachment; }

        /**
         * @brief Update the render target attachments with the ones stored in this subpass
         *        This function is called by the RenderPipeline before beginning the render
         *        pass and before proceeding with a new subpass
         */
        void update_render_target_attachments(HPPRenderTarget& render_target);

    private:
        // Default to no input attachments
        std::vector<uint32_t> input_attachments = {};

        // Default to swapchain output attachment
        std::vector<uint32_t> output_attachments = { 0 };

        // Default to no color resolve attachments
        std::vector<uint32_t> color_resolve_attachments = {};

        /**
         * @brief When creating the renderpass, pDepthStencilAttachment will
         *        be set to nullptr, which disables depth testing
         */
        bool disable_depth_stencil_attachment{ false };

        /**
         * @brief When creating the renderpass, if not None, the resolve
         *        of the multisampled depth attachment will be enabled,
         *        with this mode, to depth_stencil_resolve_attachment
         */
        vk::ResolveModeFlagBits depth_stencil_resolve_mode{ vk::ResolveModeFlagBits::eNone };

        // Default to no depth stencil resolve attachment
        uint32_t depth_stencil_resolve_attachment{ VK_ATTACHMENT_UNUSED };
    };
}