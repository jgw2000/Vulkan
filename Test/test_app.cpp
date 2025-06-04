#include "stdafx.h"
#include "test_app.h"

TestApp::TestApp(const vkb::Window::Properties& properties) :
    VulkanSample{properties}
{

}

bool TestApp::prepare()
{
    if (!VulkanSample::prepare())
    {
        return false;
    }

    auto render_pipeline = std::make_unique<vkb::rendering::HPPRenderPipeline>();
    // set_render_pipeline(std::move(render_pipeline));

    return true;
}