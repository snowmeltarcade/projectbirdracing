#pragma once

#include "iwindow_manager.h"
#include "shared/apis/logging/ilog_manager.h"

#include <cassert>

namespace pbr::shared::apis::windowing {
    /// This manager creates and manages both console and application windows
    class window_manager : public iwindow_manager {
    public:
        window_manager(std::shared_ptr<apis::logging::ilog_manager> log_manager) : _log_manager(log_manager) {
            assert((this->_log_manager));
        }
        ~window_manager() override {
            if (!this->shutdown()) {
                this->_log_manager->log_message("Failed to shutdown the window manager.", apis::logging::log_levels::error);
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

        /// Should the window manager quit?
        bool _should_quit {false};

        /// Shuts down the window manager
        /// \returns `true` upon success, else `false`
        [[nodiscard]]
        bool shutdown() noexcept;
    };
}
