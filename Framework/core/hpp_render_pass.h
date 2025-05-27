#pragma once

namespace vkb::rendering
{
    struct HPPAttachment;
}

namespace vkb::core
{
    struct HPPSubpassInfo
    {
        std::vector<uint32_t>   input_attachments;
        std::vector<uint32_t>   output_attachments;
        std::vector<uint32_t>   color_resolve_attachments;
        bool                    disable_depth_stencil_attachment;
        uint32_t                depth_stencil_resolve_attachment;
        vk::ResolveModeFlagBits depth_stencil_resolve_mode;
    };

    /**
     * @brief facade class around vkb::RenderPass, providing a vulkan.hpp-based interface
     *
     * See vkb::RenderPass for documentation
     */
    class HPPRenderPass : public VulkanResource<vk::RenderPass>
    {
    public:
        HPPRenderPass(HPPDevice&                                        device,
                      const std::vector<vkb::rendering::HPPAttachment>& attachments,
                      const std::vector<vkb::HPPLoadStoreInfo>& load_store_infos,
                      const std::vector<HPPSubpassInfo>&                subpasses);
        ~HPPRenderPass();

        HPPRenderPass(const HPPRenderPass&) = delete;
        HPPRenderPass(HPPRenderPass&&);

        HPPRenderPass& operator=(const HPPRenderPass&) = delete;
        HPPRenderPass& operator=(HPPRenderPass&&) = delete;

        const uint32_t get_color_output_count(uint32_t subpass_index) const;

        vk::Extent2D get_render_area_granularity() const;
        
    private:
        template <typename T_SubpassDescription, typename T_AttachmentDescription, typename T_AttachmentReference, typename T_SubpassDependency, typename T_RenderPassCreateInfo>
        void create_renderpass(
            const std::vector<vkb::rendering::HPPAttachment>& attachments,
            const std::vector<vkb::HPPLoadStoreInfo>& load_store_infos,
            const std::vector<HPPSubpassInfo>&                subpasses
        );

    private:
        size_t subpass_count;

        std::vector<uint32_t> color_output_count;
    };
}