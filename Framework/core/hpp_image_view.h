#pragma once

namespace vkb::core
{
    class HPPImageView : VulkanResource<vk::ImageView>
    {
    public:
        HPPImageView(HPPImage& image,
                     vk::ImageViewType view_type,
                     vk::Format        format           = vk::Format::eUndefined,
                     uint32_t          base_mip_level   = 0,
                     uint32_t          base_array_layer = 0,
                     uint32_t          n_mip_levels     = 0,
                     uint32_t          n_array_layers   = 0);

        ~HPPImageView() override;

        HPPImageView(const HPPImageView&) = delete;
        HPPImageView(HPPImageView&& other);

        HPPImageView& operator=(const HPPImageView&) = delete;
        HPPImageView& operator=(HPPImageView&&)      = delete;

        vk::Format                 get_format() const            { return format; }
        const HPPImage&            get_image() const             { return *image; }
        void                       set_image(HPPImage& img)      { image = &img; }
        vk::ImageSubresourceRange  get_subresource_range() const { return subresource_range; }
        vk::ImageSubresourceLayers get_subresource_layers() const
        {
            return vk::ImageSubresourceLayers{
                subresource_range.aspectMask, subresource_range.baseMipLevel, subresource_range.baseArrayLayer, subresource_range.layerCount
            };
        }

    private:
        HPPImage*                 image = nullptr;
        vk::Format                format;
        vk::ImageSubresourceRange subresource_range;
    };
}