#include "window_manager.h"

namespace pbr::shared::apis::windowing {
    bool window_manager::initialize() noexcept {
        this->_log_manager->log_message("Initializing the window manager...", apis::logging::log_levels::info);

        this->_log_manager->log_message("Initialized the window manager.", apis::logging::log_levels::info);
        return true;
    }

    std::shared_ptr<iconsole_window> window_manager::create_console_window() noexcept {
        return {};
    }

    bool window_manager::shutdown() noexcept {
        this->_log_manager->log_message("Shutting down the window manager...", apis::logging::log_levels::info);

        this->_log_manager->log_message("Shut down the window manager.", apis::logging::log_levels::info);
        return true;
    }
}
