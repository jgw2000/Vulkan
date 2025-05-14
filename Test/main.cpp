#include "platform/application.h"

int main() {
    vkb::Window::Properties properties =
    {
        "Sample"
    };

    vkb::Application app(properties);
    app.start();

    return 0;
}
