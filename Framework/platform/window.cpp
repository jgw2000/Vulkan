#include "window.h"

namespace vkb
{
    Window::Window(const Properties& properties) :
        properties{ properties }
    {
    }

    void Window::process_events()
    {
    }

    const Window::Extent& Window::get_extent() const
    {
        return properties.extent;
    }

    Window::Mode Window::get_window_mode() const
    {
        return properties.mode;
    }
}