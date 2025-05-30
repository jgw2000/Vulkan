#pragma once

namespace vkb::rendering
{
    class HPPSubpass;

    /**
     * @brief A RenderPipeline is a sequence of Subpass objects.
     * Subpass holds shaders and can draw the core::sg::Scene.
     * More subpasses can be added to the sequence if required.
     * For example, postprocessing can be implemented with two pipelines which
     * share render targets.
     *
     * GeometrySubpass -> Processes Scene for Shaders, use by itself if shader requires no lighting
     * ForwardSubpass -> Binds lights at the beginning of a GeometrySubpass to create Forward Rendering, should be used with most default shaders
     * LightingSubpass -> Holds a Global Light uniform, Can be combined with GeometrySubpass to create Deferred Rendering
     */
    class HPPRenderPipeline
    {
    public:
        HPPRenderPipeline(std::vector<std::unique_ptr<HPPSubpass>>&& subpasses = {});
        virtual ~HPPRenderPipeline() = default;

        HPPRenderPipeline(const HPPRenderPipeline&) = delete;
        HPPRenderPipeline(HPPRenderPipeline&&) = default;

        HPPRenderPipeline& operator=(const HPPRenderPipeline&) = delete;
        HPPRenderPipeline& operator=(HPPRenderPipeline&&) = default;

        /**
         * @brief Prepares the subpasses
         */
        void prepare();

        /**
         * @return Load store info
         */
        const std::vector<HPPLoadStoreInfo>& get_load_store() const { return load_store; }

        /**
         * @param load_store Load store info to set
         */
        void set_load_store(const std::vector<HPPLoadStoreInfo>& ls) { load_store = ls; }

        /**
         * @return Clear values
         */
        const std::vector<vk::ClearValue>& get_clear_value() const { return clear_value; }

        /**
         * @param clear_values Clear values to set
         */
        void set_clear_value(const std::vector<vk::ClearValue>& cv) { clear_value = cv; }

        /**
         * @brief Appends a subpass to the pipeline
         * @param subpass Subpass to append
         */
        void add_subpass(std::unique_ptr<HPPSubpass>&& subpass);

        std::vector<std::unique_ptr<HPPSubpass>>& get_subpasses() { return subpasses; }

        /**
         * @brief Record draw commands for each Subpass
         */
        void draw(vkb::core::HPPCommandBuffer& command_buffer, HPPRenderTarget& render_target, vk::SubpassContents contents = vk::SubpassContents::eInline);

        /**
         * @return Subpass currently being recorded, or the first one
         *         if drawing has not started
         */
        std::unique_ptr<HPPSubpass>& get_active_subpass() { return subpasses[active_subpass_index]; }

    private:
        std::vector<std::unique_ptr<HPPSubpass>> subpasses;

        // Default to two load store
        std::vector<HPPLoadStoreInfo> load_store = std::vector<HPPLoadStoreInfo>(2);

        // Default to two clear values
        std::vector<vk::ClearValue> clear_value = std::vector<vk::ClearValue>(2);

        size_t active_subpass_index{ 0 };
    };
}