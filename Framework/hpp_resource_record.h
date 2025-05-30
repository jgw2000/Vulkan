#pragma once

namespace vkb
{
    enum class ResourceType
    {
        ShaderModule,
        PipelineLayout,
        RenderPass,
        GraphicsPipeline
    };

    /**
     * @brief Writes Vulkan objects in a memory stream
     */
    class HPPResourceRecord
    {
    public:
        size_t register_render_pass(const std::vector<rendering::HPPAttachment>& attachments,
                                    const std::vector<HPPLoadStoreInfo>&         load_store_infos,
                                    const std::vector<core::HPPSubpassInfo>&     subpasses);

        void set_render_pass(size_t index, const core::HPPRenderPass& render_pass);

    private:
        std::ostringstream stream;

        std::vector<size_t> render_pass_indices;

        std::unordered_map<const core::HPPRenderPass*, size_t> render_pass_to_index;
    };
}