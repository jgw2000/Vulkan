#include "stdafx.h"
#include "application.h"
#include "glfw_window.h"

namespace vkb
{
    Application::Application(const Window::Properties& properties) :
        name{"Sample Name"}
    {
        window = std::make_unique<GlfwWindow>(properties);
    }

    void Application::start()
    {
        if (!prepare())
        {
            throw std::runtime_error("Application::prepare failed.");
        }

        if (!window)
        {
            throw std::runtime_error("Window couldn't be created.");
        }

        while (!window->should_close())
        {
            window->process_events();
            update(0.016f); // Simulate a frame time of 16ms (60 FPS)
        }

        finish();
    }

    bool Application::prepare()
    {
        return true;
    }

    void Application::finish()
    {
    }

    bool Application::resize(uint32_t /*width*/, uint32_t /*height*/)
    {
        return true;
    }

    void Application::update(float delta_time)
    {
    }

    const std::string& Application::get_name() const
    {
        return name;
    }

    void Application::set_name(const std::string& name_)
    {
        name = name_;
    }
}
