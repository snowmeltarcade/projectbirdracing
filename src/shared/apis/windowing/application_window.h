#pragma once

#include "shared/apis/logging/ilog_manager.h"
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
        /// \param title The window title
        /// \param w The width
        /// \param h The height
        application_window(std::shared_ptr<apis::logging::ilog_manager> log_manager,
                           std::string_view title,
                           uint32_t w, uint32_t h)
                           : _log_manager(log_manager) {
            assert((this->_log_manager));

            if (!this->create(title,
                              SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                              w, h)) {
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
        window_size get_window_size() const noexcept override;

    private:
        /// The log manager
        std::shared_ptr<apis::logging::ilog_manager> _log_manager;

        /// The window instance
        SDL_Window* _window {nullptr};

        /// Creates this window
        /// \param title The window title
        /// \param x The x position
        /// \param y The y position
        /// \param w The width
        /// \param h The height
        /// \returns `true` upon success, else `false`
        [[nodiscard]]
        bool create(std::string_view title,
                    uint32_t x, uint32_t y,
                    uint32_t w, uint32_t h) noexcept;

        /// Shuts down this window
        void shutdown() noexcept;
    };
}
