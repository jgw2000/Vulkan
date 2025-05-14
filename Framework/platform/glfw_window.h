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

        bool should_close() override;

        void process_events() override;

        void close() override;

    private:
        GLFWwindow* handle = nullptr;
    };
}
