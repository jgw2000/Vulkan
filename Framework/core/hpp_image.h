#pragma once

#include "allocated.h"
#include "builder_base.h"

namespace vkb::core
{
    class HPPDevice;
    class HPPImageView;
    class HPPImage;
    using HPPImagePtr = std::unique_ptr<HPPImage>;

    class HPPImageBuilder : public vkb::allocated::BuilderBase<HPPImageBuilder, vk::ImageCreateInfo>
    {
    private:
        using Parent = vkb::allocated::BuilderBase<HPPImageBuilder, vk::ImageCreateInfo>;

    public:
        HPPImageBuilder(const vk::Extent3D& extent) :       // Better reasonable defaults than vk::ImageCreateInfo default ctor
            Parent(vk::ImageCreateInfo{{}, vk::ImageType::e2D, vk::Format::eR8G8B8A8Unorm, extent, 1, 1 })
        { }

        HPPImageBuilder(const vk::Extent2D& extent) :
            HPPImageBuilder(vk::Extent3D{ extent.width, extent.height, 1 })
        { }

        HPPImageBuilder(uint32_t width, uint32_t height = 1, uint32_t depth = 1):
            HPPImageBuilder(vk::Extent3D{width, height, depth})
        { }

        HPPImageBuilder& with_format(vk::Format format)
        {
            create_info.format = format;
            return *this;
        }

        HPPImageBuilder& with_image_type(vk::ImageType type)
        {
            create_info.imageType = type;
            return *this;
        }

        HPPImageBuilder& with_array_layers(uint32_t layers)
        {
            create_info.arrayLayers = layers;
            return *this;
        }

        HPPImageBuilder& with_mip_levels(uint32_t levels)
        {
            create_info.mipLevels = levels;
            return *this;
        }

        HPPImageBuilder& with_sample_count(vk::SampleCountFlagBits sample_count)
        {
            create_info.samples = sample_count;
            return *this;
        }

        HPPImageBuilder& with_tiling(vk::ImageTiling tiling)
        {
            create_info.tiling = tiling;
            return *this;
        }

        HPPImageBuilder& with_usage(vk::ImageUsageFlags usage)
        {
            create_info.usage = usage;
            return *this;
        }

        HPPImageBuilder& with_flags(vk::ImageCreateFlags flags)
        {
            create_info.flags = flags;
            return *this;
        }

        HPPImage    build(HPPDevice& device) const;
        HPPImagePtr build_unique(HPPDevice& device) const;
    };

    class HPPImage : public vkb::allocated::Allocated<vk::Image>
    {
    public:
        HPPImage(HPPDevice&              device,
                 vk::Image               handle,
                 const vk::Extent3D&     extent,
                 vk::Format              format,
                 vk::ImageUsageFlags     image_usage,
                 vk::SampleCountFlagBits sample_count = vk::SampleCountFlagBits::e1);

        //[[deprecated("Use the HPPImageBuilder ctor instead")]]
        HPPImage(HPPDevice& device,
                 const vk::Extent3D&     extent,
                 vk::Format              format,
                 vk::ImageUsageFlags     image_usage,
                 VmaMemoryUsage          memory_usage       = VMA_MEMORY_USAGE_AUTO,
                 vk::SampleCountFlagBits sample_count       = vk::SampleCountFlagBits::e1,
                 uint32_t                mip_levels         = 1,
                 uint32_t                array_layers       = 1,
                 vk::ImageTiling         tiling             = vk::ImageTiling::eOptimal,
                 vk::ImageCreateFlags    flags              = {},
                 uint32_t                num_queue_families = 0,
                 const uint32_t*         queue_families     = nullptr);

        HPPImage(HPPDevice& device, const HPPImageBuilder& builder);

        ~HPPImage();

        HPPImage(const HPPImage&) = delete;
        HPPImage(HPPImage&& other) noexcept;

        HPPImage& operator=(const HPPImage&) = delete;
        HPPImage& operator=(HPPImage&&) = delete;

        /**
         * @brief Maps vulkan memory to an host visible address
         * @return Pointer to host visible memory
         */
        uint8_t* map();

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