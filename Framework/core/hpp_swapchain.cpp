#include "stdafx.h"

namespace vkb
{
    namespace
    {
        vk::Extent2D choose_extent(vk::Extent2D        request_extent,
                                   const vk::Extent2D& min_image_extent,
                                   const vk::Extent2D& max_image_extent,
                                   const vk::Extent2D& current_extent)
        {
            if (current_extent.width == 0xFFFFFFFF)
            {
                return request_extent;
            }

            if (request_extent.width < 1 || request_extent.height < 1)
            {
                return current_extent;
            }

            request_extent.width  = std::clamp(request_extent.width, min_image_extent.width, max_image_extent.width);
            request_extent.height = std::clamp(request_extent.height, min_image_extent.height, max_image_extent.height);

            return request_extent;
        }

        vk::PresentModeKHR choose_present_mode(vk::PresentModeKHR                     request_present_mode,
                                               const std::vector<vk::PresentModeKHR>& available_present_modes,
                                               const std::vector<vk::PresentModeKHR>& present_mode_priority_list)
        {
            // Try to find the requested present mode in the available present modes
            const auto present_mode_it = std::ranges::find(available_present_modes, request_present_mode);
            if (present_mode_it == available_present_modes.end())
            {
                // If the requested present mode isn't found, then try to find a mode from the priority list
                const auto chosen_present_mode_it =
                    std::ranges::find_if(present_mode_priority_list,
                                         [&available_present_modes](vk::PresentModeKHR present_mode) { return std::ranges::find(available_present_modes, present_mode) != available_present_modes.end(); });

                // If nothing found, always default to FIFO
                const vk::PresentModeKHR chosen_present_mode = (chosen_present_mode_it != present_mode_priority_list.end() ? *chosen_present_mode_it : vk::PresentModeKHR::eFifo);
                return chosen_present_mode;
            }
            else
            {
                return request_present_mode;
            }
        }

        vk::SurfaceFormatKHR choose_surface_format(const vk::SurfaceFormatKHR               requested_surface_format,
                                                   const std::vector<vk::SurfaceFormatKHR>& available_surface_formats,
                                                   const std::vector<vk::SurfaceFormatKHR>& surface_format_priority_list)
        {
            // Try to find the requested surface format in the available surface formats
            const auto surface_format_it = std::ranges::find(available_surface_formats, requested_surface_format);

            // If the requested surface format isn't found, then try to request a format from the priority list
            if (surface_format_it == available_surface_formats.end())
            {
                const auto chosen_surface_format_it =
                    std::ranges::find_if(surface_format_priority_list,
                                         [&available_surface_formats](vk::SurfaceFormatKHR surface_format) { return std::ranges::find(available_surface_formats, surface_format) != available_surface_formats.end(); });
                
                // If nothing found, default to the first available format
                const vk::SurfaceFormatKHR& chosen_surface_format = (chosen_surface_format_it != surface_format_priority_list.end() ? *chosen_surface_format_it : available_surface_formats[0]);
                return chosen_surface_format;
            }
            else
            {
                return requested_surface_format;
            }
        }

        vk::SurfaceTransformFlagBitsKHR choose_transform(vk::SurfaceTransformFlagBitsKHR request_transform,
                                                         vk::SurfaceTransformFlagsKHR    supported_transforms,
                                                         vk::SurfaceTransformFlagBitsKHR current_transform)
        {
            if (request_transform & supported_transforms)
            {
                return request_transform;
            }

            return current_transform;
        }

        vk::CompositeAlphaFlagBitsKHR choose_composite_alpha(vk::CompositeAlphaFlagBitsKHR request_composite_alpha,
                                                             vk::CompositeAlphaFlagsKHR    supported_composite_alpha)
        {
            if (request_composite_alpha & supported_composite_alpha)
            {
                return request_composite_alpha;
            }

            static const std::vector<vk::CompositeAlphaFlagBitsKHR> composite_alpha_priority_list = { vk::CompositeAlphaFlagBitsKHR::eOpaque,
                                                                                                      vk::CompositeAlphaFlagBitsKHR::ePreMultiplied,
                                                                                                      vk::CompositeAlphaFlagBitsKHR::ePostMultiplied,
                                                                                                      vk::CompositeAlphaFlagBitsKHR::eInherit };

            const auto chosen_composite_alpha_it =
                std::find_if(composite_alpha_priority_list.begin(),
                             composite_alpha_priority_list.end(),
                             [&supported_composite_alpha](vk::CompositeAlphaFlagBitsKHR composite_alpha) { return composite_alpha & supported_composite_alpha; });
            if (chosen_composite_alpha_it == composite_alpha_priority_list.end())
            {
                throw std::runtime_error("No compatible composite alpha found.");
            }
            else
            {
                return *chosen_composite_alpha_it;
            }
        }

        bool validate_format_feature(vk::ImageUsageFlagBits image_usage, vk::FormatFeatureFlags supported_features)
        {
            return (image_usage != vk::ImageUsageFlagBits::eStorage) || (supported_features & vk::FormatFeatureFlagBits::eStorageImage);
        }

        std::set<vk::ImageUsageFlagBits> choose_image_usage(const std::set<vk::ImageUsageFlagBits>& requested_image_usage_flags,
                                                            vk::ImageUsageFlags                     supported_image_usage,
                                                            vk::FormatFeatureFlags                  supported_features)
        {
            std::set<vk::ImageUsageFlagBits> validated_image_usage_flags;
            for (auto flag : requested_image_usage_flags)
            {
                if ((flag & supported_image_usage) && validate_format_feature(flag, supported_features))
                {
                    validated_image_usage_flags.insert(flag);
                }
            }

            if (validated_image_usage_flags.empty())
            {
                // Pick the first format from list of defaults, if supported
                static const std::vector<vk::ImageUsageFlagBits> image_usage_priority_list = {
                    vk::ImageUsageFlagBits::eColorAttachment, vk::ImageUsageFlagBits::eStorage, vk::ImageUsageFlagBits::eSampled, vk::ImageUsageFlagBits::eTransferDst };

                const auto priority_list_it =
                    std::ranges::find_if(image_usage_priority_list,
                                         [&supported_image_usage, &supported_features](auto const image_usage) { return ((image_usage & supported_image_usage) && validate_format_feature(image_usage, supported_features)); });
                if (priority_list_it != image_usage_priority_list.end())
                {
                    validated_image_usage_flags.insert(*priority_list_it);
                }
            }

            if (validated_image_usage_flags.empty())
            {
                throw std::runtime_error("No compatible image usage found.");
            }

            return validated_image_usage_flags;
        }

        vk::ImageUsageFlags composite_image_flags(std::set<vk::ImageUsageFlagBits>& image_usage_flags)
        {
            vk::ImageUsageFlags image_usage;
            for (auto flag : image_usage_flags)
            {
                image_usage |= flag;
            }
            return image_usage;
        }
    }
}

namespace vkb::core
{
    HPPSwapchain::HPPSwapchain(HPPSwapchain& old_swapchain, const vk::Extent2D& extent) :
        HPPSwapchain{old_swapchain.device,
                     old_swapchain.surface,
                     old_swapchain.properties.present_mode,
                     old_swapchain.present_mode_priority_list,
                     old_swapchain.surface_format_priority_list,
                     extent,
                     old_swapchain.properties.image_count,
                     old_swapchain.properties.pre_transform,
                     old_swapchain.image_usage_flags,
                     old_swapchain.get_handle()}
    { }

    HPPSwapchain::HPPSwapchain(HPPSwapchain& old_swapchain, uint32_t image_count) :
        HPPSwapchain{old_swapchain.device,
                     old_swapchain.surface,
                     old_swapchain.properties.present_mode,
                     old_swapchain.present_mode_priority_list,
                     old_swapchain.surface_format_priority_list,
                     old_swapchain.properties.extent,
                     image_count,
                     old_swapchain.properties.pre_transform,
                     old_swapchain.image_usage_flags,
                     old_swapchain.get_handle()}
    { }

    HPPSwapchain::HPPSwapchain(HPPSwapchain& old_swapchain, const std::set<vk::ImageUsageFlagBits>& image_usage_flags) :
        HPPSwapchain{old_swapchain.device,
                     old_swapchain.surface,
                     old_swapchain.properties.present_mode,
                     old_swapchain.present_mode_priority_list,
                     old_swapchain.surface_format_priority_list,
                     old_swapchain.properties.extent,
                     old_swapchain.properties.image_count,
                     old_swapchain.properties.pre_transform,
                     image_usage_flags,
                     old_swapchain.get_handle()}
    { }

    HPPSwapchain::HPPSwapchain(HPPSwapchain& old_swapchain, const vk::Extent2D& extent, const vk::SurfaceTransformFlagBitsKHR transform) :
        HPPSwapchain{old_swapchain.device,
                     old_swapchain.surface,
                     old_swapchain.properties.present_mode,
                     old_swapchain.present_mode_priority_list,
                     old_swapchain.surface_format_priority_list,
                     extent,
                     old_swapchain.properties.image_count,
                     transform,
                     old_swapchain.image_usage_flags,
                     old_swapchain.get_handle()}
    { }

    HPPSwapchain::HPPSwapchain(HPPDevice& device,
                               vk::SurfaceKHR                           surface,
                               const vk::PresentModeKHR                 present_mode,
                               const std::vector<vk::PresentModeKHR>&   present_mode_priority_list,
                               const std::vector<vk::SurfaceFormatKHR>& surface_format_priority_list,
                               const vk::Extent2D&                      extent,
                               const uint32_t                           image_count,
                               const vk::SurfaceTransformFlagBitsKHR    transform,
                               const std::set<vk::ImageUsageFlagBits>&  image_usage_flags,
                               vk::SwapchainKHR                         old_swapchain) :
        device{ device },
        surface{ surface }
    {
        this->present_mode_priority_list   = present_mode_priority_list;
        this->surface_format_priority_list = surface_format_priority_list;

        auto surface_formats = device.get_gpu().get_handle().getSurfaceFormatsKHR(surface);
        auto present_modes = device.get_gpu().get_handle().getSurfacePresentModesKHR(surface);

        // Choose best properties based on surface capabilities
        const auto surface_capabilities = device.get_gpu().get_handle().getSurfaceCapabilitiesKHR(surface);

        properties.old_swapchain     = old_swapchain;
        properties.image_count       = std::clamp(image_count, surface_capabilities.minImageCount,
                                            surface_capabilities.maxImageCount ? surface_capabilities.maxImageCount : std::numeric_limits<uint32_t>::max());
        properties.extent            = choose_extent(extent, surface_capabilities.minImageExtent, surface_capabilities.maxImageExtent, surface_capabilities.currentExtent);
        properties.surface_format    = choose_surface_format(properties.surface_format, surface_formats, surface_format_priority_list);
        properties.array_layers      = 1;

        const auto format_properties = device.get_gpu().get_handle().getFormatProperties(properties.surface_format.format);
        this->image_usage_flags      = choose_image_usage(image_usage_flags, surface_capabilities.supportedUsageFlags, format_properties.optimalTilingFeatures);

        properties.image_usage       = composite_image_flags(this->image_usage_flags);
        properties.pre_transform     = choose_transform(transform, surface_capabilities.supportedTransforms, surface_capabilities.currentTransform);
        properties.composite_alpha   = choose_composite_alpha(vk::CompositeAlphaFlagBitsKHR::eInherit, surface_capabilities.supportedCompositeAlpha);
        properties.present_mode      = choose_present_mode(present_mode, present_modes, present_mode_priority_list);

        const vk::SwapchainCreateInfoKHR create_info{
            {},
            surface,
            properties.image_count,
            properties.surface_format.format,
            properties.surface_format.colorSpace,
            properties.extent,
            properties.array_layers,
            properties.image_usage,
            VULKAN_HPP_NAMESPACE::SharingMode::eExclusive,
            {},
            {},
            properties.pre_transform,
            properties.composite_alpha,
            properties.present_mode,
            {},
            properties.old_swapchain
        };

        handle = device.get_handle().createSwapchainKHR(create_info);

        images = device.get_handle().getSwapchainImagesKHR(handle);
    }

    HPPSwapchain::HPPSwapchain(HPPSwapchain&& other) :
        device{ other.device },
        surface{ std::exchange(other.surface, nullptr) },
        handle{ std::exchange(other.handle, nullptr) },
        images{ std::exchange(other.images, {}) },
        properties{ std::exchange(other.properties, {}) },
        present_mode_priority_list{ std::exchange(other.present_mode_priority_list, {}) },
        surface_format_priority_list{ std::exchange(other.surface_format_priority_list, {}) },
        image_usage_flags{ std::move(other.image_usage_flags) }
    { }

    HPPSwapchain::~HPPSwapchain()
    {
        if (handle)
        {
            device.get_handle().destroySwapchainKHR(handle);
        }
    }
}