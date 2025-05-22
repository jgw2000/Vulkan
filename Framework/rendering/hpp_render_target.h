#pragma once

namespace vkb::rendering
{
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

        const vk::Extent2D&                    get_extent() const { return extent; }
        const std::vector<core::HPPImageView>& get_views() const { return views; }

    private:
        const core::HPPDevice&          device;
        vk::Extent2D                    extent;
        std::vector<core::HPPImage>     images;
        std::vector<core::HPPImageView> views;
    };
}