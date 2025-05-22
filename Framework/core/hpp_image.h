#pragma once

#include "allocated.h"

namespace vkb::core
{
    class HPPImageView;

    class HPPImage : public vkb::allocated::Allocated<vk::Image>
    {
    public:
        HPPImage(HPPDevice&              device,
                 vk::Image               handle,
                 const vk::Extent3D&     extent,
                 vk::Format              format,
                 vk::ImageUsageFlags     image_usage,
                 vk::SampleCountFlagBits sample_count = vk::SampleCountFlagBits::e1);
        ~HPPImage();

        HPPImage(const HPPImage&) = delete;
        HPPImage(HPPImage&& other) noexcept;

        HPPImage& operator=(const HPPImage&) = delete;
        HPPImage& operator=(HPPImage&&) = delete;

        vk::ImageType                      get_type() const              { return create_info.imageType; }
        const vk::Extent3D&                get_extent() const            { return create_info.extent; }
        vk::Format                         get_format() const            { return create_info.format; }
        vk::SampleCountFlagBits            get_sample_count() const      { return create_info.samples; }
        vk::ImageUsageFlags                get_usage() const             { return create_info.usage; }
        vk::ImageTiling                    get_tiling() const            { return create_info.tiling; }
        vk::ImageSubresource               get_subresource() const       { return subresource; }
        uint32_t                           get_array_layer_count() const { return create_info.arrayLayers; }
        std::unordered_set<HPPImageView*>& get_views()                   { return views; }

    private:
        vk::ImageCreateInfo               create_info;
        vk::ImageSubresource              subresource;
        std::unordered_set<HPPImageView*> views;        /// HPPImage views referring to this image
    };
}