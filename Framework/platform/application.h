#pragma once

#include "common/vk_common.h"
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
        virtual bool resize(const uint32_t width, const uint32_t height);

        std::unique_ptr<Window> window{ nullptr };

    private:
        float fps{ 0.0f };
        float frame_time{ 0.0f };
        uint32_t frame_count{ 0 };
    };
}
