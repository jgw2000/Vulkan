#pragma once

#include "window.h"

struct GLFWwindow;

namespace vkb
{
    /**
    * @brief An implementation of GLFW, inheriting the behaviour of the Window interface
    */
    class GlfwWindow : public Window
    {
    public:
        GlfwWindow(const Window::Properties& properties);
        virtual ~GlfwWindow();

        VkSurfaceKHR create_surface(VkInstance instance) override;

        bool should_close() override;

        void process_events() override;

        void close() override;

        std::vector<const char*> get_required_surface_extensions() const override;

    private:
        GLFWwindow* handle = nullptr;
    };
}
