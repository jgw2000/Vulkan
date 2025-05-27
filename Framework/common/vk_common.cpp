#include "stdafx.h"

namespace vkb
{
    bool is_depth_only_format(vk::Format format)
    {
        return format == vk::Format::eD16Unorm ||
               format == vk::Format::eD32Sfloat;
    }

    bool is_depth_stencil_format(vk::Format format)
    {
        return format == vk::Format::eD16UnormS8Uint ||
               format == vk::Format::eD24UnormS8Uint ||
               format == vk::Format::eD32SfloatS8Uint;
    }

    bool is_depth_format(vk::Format format)
    {
        return is_depth_only_format(format) || is_depth_stencil_format(format);
    }

    vk::Format get_suitable_depth_format(
        vk::PhysicalDevice             physical_device,
        bool                           depth_only,
        const std::vector<vk::Format>& depth_format_priority_list
    )
    {
        vk::Format depth_format{ vk::Format::eUndefined };
        
        for (auto& format : depth_format_priority_list)
        {
            if (depth_only && !is_depth_only_format(format))
            {
                continue;
            }

            vk::FormatProperties properties = physical_device.getFormatProperties(format);

            // Format must support depth stencil attachment for optimal tiling
            if (properties.optimalTilingFeatures & vk::FormatFeatureFlagBits::eDepthStencilAttachment)
            {
                depth_format = format;
                break;
            }
        }

        if (depth_format != vk::Format::eUndefined)
        {
            return depth_format;
        }

        throw std::runtime_error("No suitable depth format could be determined");
    }
}