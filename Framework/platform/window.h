#pragma once

#include "common/vk_common.h"

namespace vkb
{
    /**
    * @brief An interface class, declaring the behaviour of a Window
    */
    class Window
    {
    public:
        struct Extent
        {
            uint32_t width;
            uint32_t height;
        };

        enum class Mode
        {
            Headless,
            Fullscreen,
            FullscreenBorderless,
            FullscreenStretch,
            Default
        };

        enum class Vsync
        {
            OFF,
            ON,
            Default
        };

        struct Properties
        {
            std::string title     = "";
            Mode        mode      = Mode::Default;
            bool        resizable = true;
            Vsync       vsync     = Vsync::Default;
            Extent      extent    = { 1280, 720 };
        };

        /**
        * @brief Constructs a Window
        * @param properties The preferred configuration of the window
        */
        Window(const Properties& properties);

        virtual ~Window() = default;

        /**
        * @brief Checks if the window should be closed
        */
        virtual bool should_close() = 0;

        /**
         * @brief Handles the processing of all underlying window events
         */
        virtual void process_events();

        /**
         * @brief Requests to close the window
         */
        virtual void close() = 0;

        const Extent& get_extent() const;

        Mode get_window_mode() const;

        inline const Properties& get_properties() const
        {
            return properties;
        }

    protected:
        Properties properties;
    };
}