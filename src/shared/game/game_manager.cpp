#include "game_manager.h"

namespace pbr::shared::game {
    bool game_manager::initialize() noexcept {
        this->_log_manager->log_message("Initializing the game manager...", apis::logging::log_levels::info);

        if (!this->_window_manager->initialize()) {
            this->_log_manager->log_message("Failed to initialize window manager.", apis::logging::log_levels::error);
            return false;
        }

        this->_application_window = this->_window_manager->create_application_window();
        if (!this->_application_window) {
            this->_log_manager->log_message("Failed to create application window.", apis::logging::log_levels::error);
            return false;
        }

        if (!this->_scene_manager->initialize()) {
            this->_log_manager->log_message("Failed to initialize scene manager.", apis::logging::log_levels::error);
            return false;
        }

        this->_log_manager->log_message("Initialized the game manager.", apis::logging::log_levels::info);

        return true;
    }

    bool game_manager::run() noexcept {
        this->_log_manager->log_message("Running the game manager...", apis::logging::log_levels::info);

        while (!this->_has_exit_been_requested) {
            if (!this->_window_manager->update()) {
                this->_log_manager->log_message("Failed to update window manager.", apis::logging::log_levels::error);
                return false;
            }

            if (this->_window_manager->should_quit()) {
                this->request_exit();
                continue;
            }

            if (!this->_scene_manager->run()) {
                this->_log_manager->log_message("Failed to run scene manager.", apis::logging::log_levels::error);
                return false;
            }
        }

        this->_log_manager->log_message("Finished running the game manager.", apis::logging::log_levels::info);

        return true;
    }

    bool game_manager::shutdown() noexcept {
        this->_log_manager->log_message("Shutting down the game manager...", apis::logging::log_levels::info);

        this->_log_manager->log_message("Shut down the game manager.", apis::logging::log_levels::info);

        return true;
    }
}
