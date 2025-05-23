#pragma once

namespace vkb::rendering
{
    /**
     * @brief Description of render pass attachments.
     * HPPAttachment descriptions can be used to automatically create render target images.
     */
    struct HPPAttachment
    {
        HPPAttachment() = default;

        HPPAttachment(vk::Format format, vk::SampleCountFlagBits samples, vk::ImageUsageFlags usage) :
            format{format},
            samples{samples},
            usage{usage}
        { }

        vk::Format              format         = vk::Format::eUndefined;
        vk::SampleCountFlagBits samples        = vk::SampleCountFlagBits::e1;
        vk::ImageUsageFlags     usage          = vk::ImageUsageFlagBits::eSampled;
        vk::ImageLayout         initial_layout = vk::ImageLayout::eUndefined;
    };

    class HPPRenderTarget
    {
    public:
        using CreateFunc = std::function<std::unique_ptr<HPPRenderTarget>(core::HPPImage&&)>;

        const static CreateFunc DEFAULT_CREATE_FUNC;

        HPPRenderTarget(std::vector<core::HPPImage>&& images);
        HPPRenderTarget(std::vector<core::HPPImageView>&& image_views);

        HPPRenderTarget(const HPPRenderTarget&) = delete;
        HPPRenderTarget(HPPRenderTarget&&) = delete;
        HPPRenderTarget& operator=(const HPPRenderTarget&) noexcept = delete;
        HPPRenderTarget& operator=(HPPRenderTarget&&) noexcept = delete;

        const vk::Extent2D&                    get_extent() const      { return extent; }
        const std::vector<core::HPPImageView>& get_views() const       { return views; }
        const std::vector<HPPAttachment>&      get_attachments() const { return attachments; }

        /**
         * @brief Sets the current input attachments overwriting the current ones
         *        Should be set before beginning the render pass and before starting a new subpass
         * @param input Set of attachment reference number to use as input
         */
        void                         set_input_attachments(std::vector<uint32_t>& input);
        const std::vector<uint32_t>& get_input_attachments() const;

        /**
         * @brief Sets the current output attachments overwriting the current ones
         *        Should be set before beginning the render pass and before starting a new subpass
         * @param output Set of attachment reference number to use as output
         */
        void                         set_output_attachments(std::vector<uint32_t>& output);
        const std::vector<uint32_t>& get_output_attachments() const;
        void                         set_layout(uint32_t attachment, vk::ImageLayout layout);
        vk::ImageLayout              get_layout(uint32_t attachment) const;

    private:
        const core::HPPDevice&          device;
        vk::Extent2D                    extent;
        std::vector<core::HPPImage>     images;
        std::vector<core::HPPImageView> views;
        std::vector<HPPAttachment>      attachments;
        std::vector<uint32_t>           input_attachments = {};         // By default there are no input attachments
        std::vector<uint32_t>           output_attachments = { 0 };     // By default the output attachments is attachment 0
    };
}