#pragma once

#include "iwindow_manager.h"
#include "shared/apis/logging/ilog_manager.h"
#include "shared/apis/graphics/apis.h"

#include <cassert>

namespace pbr::shared::apis::windowing {
    /// This manager creates and manages both console and application windows
    class window_manager : public iwindow_manager {
    public:
        window_manager(std::shared_ptr<apis::logging::ilog_manager> log_manager,
                       graphics::apis graphics_api)
                       : _log_manager(log_manager),
                            _graphics_api(graphics_api) {
            assert((this->_log_manager));
        }
        ~window_manager() override {
            if (!this->shutdown()) {
                this->_log_manager->log_message("Failed to shutdown the window manager.",
                                                apis::logging::log_levels::error,
                                                "Windowing");
            }
        }

        /// Initializes the window manager
        /// \returns `true` upon success, else `false`
        [[nodiscard]]
        bool initialize() noexcept override;

        /// Creates a console window
        /// \returns The created console window
        [[nodiscard]]
        std::shared_ptr<iconsole_window> create_console_window() noexcept override;

        /// Creates an application window
        /// \returns The created application window
        [[nodiscard]]
        std::shared_ptr<iapplication_window> create_application_window() noexcept override;

        /// Returns the main application window
        /// \returns The main application window, else `nullptr` is no application window has been created
        [[nodiscard]]
        std::shared_ptr<iapplication_window> get_main_application_window() const noexcept override {
            return this->_application_windows.empty() ? nullptr : this->_application_windows[0];
        }

        /// Updates the windowing system
        /// \returns `true` upon success, else `false`
        [[nodiscard]]
        bool update() noexcept override;

        /// Returns true if the windowing system has a quit event
        /// \returns `true` if the windowing system has a quit event, else `false`
        [[nodiscard]]
        bool should_quit() const noexcept override {
            return this->_should_quit;
        }

    private:
        /// The log manager
        std::shared_ptr<apis::logging::ilog_manager> _log_manager;

        /// The created application windows
        std::vector<std::shared_ptr<iapplication_window>> _application_windows;

        /// Should the window manager quit?
        bool _should_quit {false};

        /// The apis the graphics manager is using
        graphics::apis _graphics_api;

        /// Shuts down the window manager
        /// \returns `true` upon success, else `false`
        [[nodiscard]]
        bool shutdown() noexcept;
    };
}
