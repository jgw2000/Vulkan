#include "stdafx.h"
#include "vulkan_sample.h"

int main() {
    vkb::Window::Properties properties =
    {
        "Sample"
    };

    vkb::VulkanSample app(properties);
    app.start();

    return 0;
}
