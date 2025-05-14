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

    Window::Extent Window::resize(const Extent& new_extent)
    {
        if (properties.resizable)
        {
            properties.extent.width = new_extent.width;
            properties.extent.height = new_extent.height;
        }

        return properties.extent;
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
