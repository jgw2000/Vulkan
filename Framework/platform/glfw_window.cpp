#include "stdafx.h"
#include "glfw_window.h"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

namespace vkb
{
    GlfwWindow::GlfwWindow(const Window::Properties& properties) :
        Window(properties)
    {
        if (!glfwInit())
        {
            throw std::runtime_error("GLFW couldn't be initialized.");
        }

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

        switch (properties.mode)
        {
            case Window::Mode::Fullscreen:
            {
                auto* monitor    = glfwGetPrimaryMonitor();
                const auto* mode = glfwGetVideoMode(monitor);
                handle           = glfwCreateWindow(mode->width, mode->height, properties.title.c_str(), monitor, NULL);
                break;
            }

            case Window::Mode::FullscreenBorderless:
            {
                auto* monitor    = glfwGetPrimaryMonitor();
                const auto* mode = glfwGetVideoMode(monitor);
                glfwWindowHint(GLFW_RED_BITS, mode->redBits);
                glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
                glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
                glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);
                handle = glfwCreateWindow(mode->width, mode->height, properties.title.c_str(), monitor, NULL);
                break;
            }

            case Window::Mode::FullscreenStretch:
            {
                throw std::runtime_error("Cannot support stretch mode on this platform.");
                break;
            }

            default:
                handle = glfwCreateWindow(properties.extent.width, properties.extent.height, properties.title.c_str(), nullptr, nullptr);
                break;
        }

        if (!handle)
        {
            throw std::runtime_error("Couldn't create glfw window.");
        }
    }

    GlfwWindow::~GlfwWindow()
    {
        glfwDestroyWindow(handle);
        glfwTerminate();
    }

    bool GlfwWindow::should_close()
    {
        return glfwWindowShouldClose(handle);
    }

    void GlfwWindow::process_events()
    {
        glfwPollEvents();
    }

    void GlfwWindow::close()
    {
        glfwSetWindowShouldClose(handle, GLFW_TRUE);
    }

    std::vector<const char*> GlfwWindow::get_required_surface_extensions() const
    {
        uint32_t glfw_extension_count{ 0 };
        const char** names = glfwGetRequiredInstanceExtensions(&glfw_extension_count);
        return { names, names + glfw_extension_count };
    }
}
