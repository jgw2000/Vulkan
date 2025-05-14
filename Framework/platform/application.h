#pragma once

#include "window.h"

namespace vkb
{
    class Application
    {
    public:
        Application(const Window::Properties& properties);
        virtual ~Application() = default;

        void start();

    protected:
        /**
        * @brief Prepares the application for execution
        */
        virtual bool prepare();

        /**
         * @brief Updates the application
         * @param delta_time The time since the last update
         */
        virtual void update(float delta_time);

        /**
         * @brief Handles cleaning up the application
         */
        virtual void finish();

        /**
         * @brief Handles resizing of the window
         * @param width New width of the window
         * @param height New height of the window
         */
        virtual bool resize(uint32_t width, uint32_t height);

        const std::string& get_name() const;

        void set_name(const std::string& name);

        std::unique_ptr<Window> window{ nullptr };

    private:
        std::string name{};
    };
}
