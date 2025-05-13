#include "application.h"
#include "glfw_window.h"

namespace vkb
{
    Application::Application(const Window::Properties& properties)
    {
        window = std::make_unique<GlfwWindow>(properties);
    }

    void Application::start()
    {
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

    void Application::update(float delta_time)
    {
        fps        = 1.0f / delta_time;
        frame_time = delta_time * 1000.0f;
    }
}