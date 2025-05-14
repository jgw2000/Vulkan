#define VULKAN_HPP_DISPATCH_LOADER_DYNAMIC 1

#include "stdafx.h"
#include "vulkan_sample.h"

VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE

int main() {
    vkb::Window::Properties properties =
    {
        "Sample"
    };

    vkb::VulkanSample app(properties);
    app.start();

    return 0;
}
