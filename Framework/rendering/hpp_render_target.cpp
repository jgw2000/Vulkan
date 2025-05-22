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

        // TODO
    }

    HPPRenderTarget::HPPRenderTarget(std::vector<core::HPPImageView>&& image_views) :
        device{image_views.back().get_image().get_device()},
        views{std::move(image_views)}
    {
        assert(!views.empty() && "Should specify at least 1 image view");

        // TODO
    }
}