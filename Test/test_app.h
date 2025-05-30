#pragma once

#include "vulkan_sample.h"

class TestApp : public vkb::VulkanSample
{
public:
    TestApp(const vkb::Window::Properties& properties);
    virtual ~TestApp() = default;

    virtual bool prepare() override;
};