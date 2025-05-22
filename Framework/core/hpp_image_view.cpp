#include "stdafx.h"

namespace vkb::core
{
    HPPImageView::HPPImageView(HPPImage& img,
                               vk::ImageViewType view_type,
                               vk::Format        format,
                               uint32_t          base_mip_level,
                               uint32_t          base_array_layer,
                               uint32_t          n_mip_levels,
                               uint32_t          n_array_layers) :
        VulkanResource{ nullptr, &img.get_device() }, image{ &img }, format{ format }
    {
        if (format == vk::Format::eUndefined)
        {
            this->format = format = image->get_format();
        }

        subresource_range = vk::ImageSubresourceRange{
            std::string(vk::componentName(format, 0)) == "D" ? vk::ImageAspectFlagBits::eDepth : vk::ImageAspectFlagBits::eColor,
            base_mip_level,
            n_mip_levels == 0 ? image->get_subresource().mipLevel : n_mip_levels,
            base_array_layer,
            n_array_layers == 0 ? image->get_subresource().arrayLayer : n_array_layers,
        };

        vk::ImageViewCreateInfo image_view_create_info{
            {},
            image->get_handle(),
            view_type,
            format,
            {},
            subresource_range
        };

        set_handle(get_device().get_handle().createImageView(image_view_create_info));

        // Register this image view to its image
        // in order to be notified when it gets moved
        image->get_views().emplace(this);
    }

    HPPImageView::~HPPImageView()
    {
        if (get_handle())
        {
            get_device().get_handle().destroyImageView(get_handle());
        }
    }

    HPPImageView::HPPImageView(HPPImageView&& other) :
        VulkanResource{ std::move(other) }, image{ other.image }, format{ other.format }, subresource_range{ other.subresource_range }
    {
        // Remove old view from image set and add this new one
        auto& views = image->get_views();
        views.erase(&other);
        views.emplace(this);

        other.set_handle(nullptr);
    }
}