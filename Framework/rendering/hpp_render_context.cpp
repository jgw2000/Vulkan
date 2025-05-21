#include "stdafx.h"

namespace vkb::rendering
{
    vk::Format HPPRenderContext::DEFAULT_VK_FORMAT = vk::Format::eR8G8B8A8Srgb;

    HPPRenderContext::HPPRenderContext(vkb::core::HPPDevice&                   device,
                                      vk::SurfaceKHR                           surface,
                                      const vkb::Window&                       window,
                                      vk::PresentModeKHR                       present_mode,
                                      const std::vector<vk::PresentModeKHR>&   present_mode_priority_list,
                                      const std::vector<vk::SurfaceFormatKHR>& surface_format_priority_list) :
        device{ device }, window{ window }, queue{ device.get_suitable_graphics_queue() }, surface_extent{window.get_extent().width, window.get_extent().height}
    {
        if (surface)
        {
            auto surface_properties = device.get_gpu().get_handle().getSurfaceCapabilitiesKHR(surface);

            if (surface_properties.currentExtent.width = 0xFFFFFFFF)
            {
                swapchain =
                    std::make_unique<vkb::core::HPPSwapchain>(device, surface, present_mode, present_mode_priority_list, surface_format_priority_list, surface_extent);
            }
            else
            {
                swapchain = std::make_unique<vkb::core::HPPSwapchain>(device, surface, present_mode, present_mode_priority_list, surface_format_priority_list);
            }
        }
    }
}
