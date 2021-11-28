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

    private:
        /// The log manager
        std::shared_ptr<apis::logging::ilog_manager> _log_manager;

        /// Shuts down the window manager
        [[nodiscard]]
        bool shutdown() noexcept;
    };
}
