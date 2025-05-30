#define VULKAN_HPP_DISPATCH_LOADER_DYNAMIC 1

#include "stdafx.h"
#include "test_app.h"

VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE

int main() {
    vkb::Window::Properties properties =
    {
        "Sample"
    };

    TestApp app(properties);
    app.start();

    return 0;
}
