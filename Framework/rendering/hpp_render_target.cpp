#include "stdafx.h"

namespace vkb::rendering
{
    const HPPRenderTarget::CreateFunc HPPRenderTarget::DEFAULT_CREATE_FUNC = [](core::HPPImage&& swapchain_image) -> std::unique_ptr<HPPRenderTarget> {
        vk::Format depth_format = vkb::common::get_suitable_depth_format(swapchain_image.get_device().get_gpu().get_handle());

        vkb::core::HPPImage depth_image{ swapchain_image.get_device(), swapchain_image.get_extent(),
                                    depth_format,
                                    vk::ImageUsageFlagBits::eDepthStencilAttachment | vk::ImageUsageFlagBits::eTransientAttachment,
                                    VMA_MEMORY_USAGE_GPU_ONLY };

        std::vector<core::HPPImage> images;
        images.push_back(std::move(swapchain_image));
        images.push_back(std::move(depth_image));

        return std::make_unique<HPPRenderTarget>(std::move(images));
    };

    HPPRenderTarget::HPPRenderTarget(std::vector<core::HPPImage>&& images_) :
        device{images_.back().get_device()},
        images{std::move(images_)}
    {
        assert(!images.empty() && "Should specify at least 1 image");

        // check that every image is 2D
        auto it = std::ranges::find_if(images, [](const core::HPPImage& image) {
            return image.get_type() != vk::ImageType::e2D;
        });
        if (it != images.end())
        {
            throw std::runtime_error("Image type is not 2D");
        }

        extent.width  = images.front().get_extent().width;
        extent.height = images.front().get_extent().height;

        // check that every image has the same extent
        it = std::find_if(std::next(images.begin()), images.end(),
            [this](const core::HPPImage& image) { return (extent.width != image.get_extent().width) || (extent.height != image.get_extent().height); });
        if (it != images.end())
        {
            throw std::runtime_error("Extent size is not same");
        }

        for (auto& image : images)
        {
            views.emplace_back(image, vk::ImageViewType::e2D);
            attachments.emplace_back(HPPAttachment{ image.get_format(), image.get_sample_count(), image.get_usage() });
        }
    }

    HPPRenderTarget::HPPRenderTarget(std::vector<core::HPPImageView>&& image_views) :
        device{image_views.back().get_image().get_device()},
        views{std::move(image_views)}
    {
        assert(!views.empty() && "Should specify at least 1 image view");

        uint32_t mip_level = views.front().get_subresource_range().baseMipLevel;
        extent.width       = views.front().get_image().get_extent().width >> mip_level;
        extent.height      = views.front().get_image().get_extent().height >> mip_level;

        // check that every image view has the same extent
        auto it = std::find_if(std::next(views.begin()), views.end(),
            [this](const core::HPPImageView& image_view) {
                const uint32_t mip_level = image_view.get_subresource_range().baseMipLevel;
                return (extent.width != image_view.get_image().get_extent().width >> mip_level) ||
                       (extent.height != image_view.get_image().get_extent().height >> mip_level);
            });
        if (it != views.end())
        {
            throw std::runtime_error("Extent size is not same");
        }

        for (auto& view : views)
        {
            const auto& image = view.get_image();
            attachments.emplace_back(HPPAttachment{ image.get_format(), image.get_sample_count(), image.get_usage() });
        }
    }

    void HPPRenderTarget::set_input_attachments(std::vector<uint32_t>& input)
    {
        input_attachments = input;
    }

    const std::vector<uint32_t>& HPPRenderTarget::get_input_attachments() const
    {
        return input_attachments;
    }

    void HPPRenderTarget::set_output_attachments(std::vector<uint32_t>& output)
    {
        output_attachments = output;
    }

    const std::vector<uint32_t>& HPPRenderTarget::get_output_attachments() const
    {
        return output_attachments;
    }

    void HPPRenderTarget::set_layout(uint32_t attachment, vk::ImageLayout layout)
    {
        attachments[attachment].initial_layout = layout;
    }

    vk::ImageLayout HPPRenderTarget::get_layout(uint32_t attachment) const
    {
        return attachments[attachment].initial_layout;
    }
}