#include "stdafx.h"

namespace vkb
{
    namespace
    {
        inline vk::ImageType find_image_type(const vk::Extent3D& extent)
        {
            uint32_t dim_num = !!extent.width + !!extent.height + (1 < extent.depth);
            switch (dim_num)
            {
            case 1:
                return vk::ImageType::e1D;
            case 2:
                return vk::ImageType::e2D;
            case 3:
                return vk::ImageType::e3D;
            default:
                throw std::runtime_error("No image type found.");
                return vk::ImageType();
            }
        }
    }
}

namespace vkb::core
{
    HPPImage HPPImageBuilder::build(HPPDevice& device) const
    {
        return HPPImage(device, *this);
    }

    HPPImagePtr HPPImageBuilder::build_unique(HPPDevice& device) const
    {
        return std::make_unique<HPPImage>(device, *this);
    }

    HPPImage::HPPImage(HPPDevice&              device,
                       vk::Image               handle,
                       const vk::Extent3D&     extent,
                       vk::Format              format,
                       vk::ImageUsageFlags     image_usage,
                       vk::SampleCountFlagBits sample_count) :
        vkb::allocated::Allocated<vk::Image>{handle, &device}
    {
        create_info.samples     = sample_count;
        create_info.format      = format;
        create_info.extent      = extent;
        create_info.imageType   = find_image_type(extent);
        create_info.arrayLayers = 1;
        create_info.mipLevels   = 1;
        subresource.mipLevel    = 1;
        subresource.arrayLayer  = 1;
    }

    HPPImage::HPPImage(HPPDevice& device,
                       const vk::Extent3D&     extent,
                       vk::Format              format,
                       vk::ImageUsageFlags     image_usage,
                       VmaMemoryUsage          memory_usage,
                       vk::SampleCountFlagBits sample_count,
                       uint32_t                mip_levels,
                       uint32_t                array_layers,
                       vk::ImageTiling         tiling,
                       vk::ImageCreateFlags    flags,
                       uint32_t                num_queue_families,
                       const uint32_t*         queue_families) :
        HPPImage{device,
                 HPPImageBuilder{extent}
                     .with_format(format)
                     .with_mip_levels(mip_levels)
                     .with_array_layers(array_layers)
                     .with_sample_count(sample_count)
                     .with_tiling(tiling)
                     .with_flags(flags)
                     .with_usage(image_usage)
                     .with_queue_families(num_queue_families, queue_families)}
    { }

    HPPImage::HPPImage(HPPDevice& device, const HPPImageBuilder& builder):
        vkb::allocated::Allocated<vk::Image>{ builder.get_allocation_create_info(), nullptr, &device }, create_info{ builder.get_create_info() }
    {
        get_handle() = create_image(create_info);
        subresource.arrayLayer = create_info.arrayLayers;
        subresource.mipLevel = create_info.mipLevels;
    }

    HPPImage::~HPPImage()
    {
        destroy_image(get_handle());
    }

    HPPImage::HPPImage(HPPImage&& other) noexcept :
        vkb::allocated::Allocated<vk::Image>{ std::move(other) },
        create_info(std::exchange(other.create_info, {})),
        subresource(std::exchange(other.subresource, {})),
        views(std::exchange(other.views, {}))
    {
        // Update image views reference to this image to avoid dangling pointers
        for (auto& view : views)
        {
            view->set_image(*this);
        }
    }

    uint8_t* HPPImage::map()
    {
        return vkb::allocated::Allocated<vk::Image>::map();
    }
}