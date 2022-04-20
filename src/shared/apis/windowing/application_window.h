#pragma once

#include "shared/apis/logging/ilog_manager.h"
#include "shared/apis/graphics/apis.h"
#include "iapplication_window.h"

#include <stdexcept>
#include <cassert>
#include <string>
#include <SDL.h>

namespace pbr::shared::apis::windowing {
    /// An application window can either be in windowed or full screen mode. This window will
    /// contain the render surface
    class application_window : public iapplication_window {
    public:
        /// Constructs this window
        /// \param log_manager The log manager to use
        /// \param graphics_api The graphics API to create this window for
        /// \param title The window title
        /// \param width The width
        /// \param height The height
        /// \param fullscreen If this window should be full screen or not
        application_window(std::shared_ptr<apis::logging::ilog_manager> log_manager,
                           graphics::apis graphics_api,
                           std::string_view title,
                           pixels width, pixels height,
                           bool fullscreen)
                           : _log_manager(log_manager),
                             _graphics_api(graphics_api) {
            assert((this->_log_manager));

            if (!this->create(title,
                              SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                              width, height,
                              fullscreen)) {
                auto message = "Failed to create application window.";
                this->_log_manager->log_message(message,
                                                apis::logging::log_levels::fatal,
                                                "Windowing");
                throw std::logic_error(message);
            }
        }

        /// Destroys this window
        ~application_window() override {
            this->shutdown();
        }

        /// Returns the native window handle
        /// \returns The native window handle
        [[nodiscard]]
        SDL_Window* get_native_handle() const noexcept {
            return this->_window;
        }

        /// Returns the window size
        /// \returns The window size
        [[nodiscard]]
        window_size get_size() const noexcept override;

        /// Sets the window size
        /// \param width The window width
        /// \param height The window height
        /// \param fullscreen If this window should be full screen or not
        /// \returns `true` upon success, else `false`
        [[nodiscard]]
        bool set_size(pixels width, pixels height, bool fullscreen) noexcept override;

    private:
        /// The log manager
        std::shared_ptr<apis::logging::ilog_manager> _log_manager;

        /// The graphics api this window is created for
        graphics::apis _graphics_api;

        /// The window instance
        SDL_Window* _window {nullptr};

        /// Creates this window
        /// \param title The window title
        /// \param x The x position
        /// \param y The y position
        /// \param width The width
        /// \param height The height
        /// \param fullscreen `true` if this window should be full screen, else `false`
        /// \returns `true` upon success, else `false`
        [[nodiscard]]
        bool create(std::string_view title,
                    pixels x, pixels y,
                    pixels width, pixels height,
                    bool fullscreen) noexcept;

        /// Shuts down this window
        void shutdown() noexcept;
    };
}
