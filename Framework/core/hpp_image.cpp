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

    HPPImage::~HPPImage()
    {
        // TODO
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
}