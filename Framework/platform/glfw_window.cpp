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

        handle = glfwCreateWindow(properties.extent.width, properties.extent.height, properties.title.c_str(), nullptr, nullptr);
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
}