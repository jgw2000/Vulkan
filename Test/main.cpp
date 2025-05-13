#include "platform/application.h"

int main() {
    vkb::Window::Properties properties;
    vkb::Application app(properties);
    app.start();

    return 0;
}