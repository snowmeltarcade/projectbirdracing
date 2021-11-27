#include "game_manager.h"

namespace pbr::shared::game {
    bool game_manager::initialize() noexcept {
        this->_log_manager->log_message("Initializing the game manager...", apis::logging::log_levels::info);

        this->_log_manager->log_message("Initialized the game manager.", apis::logging::log_levels::info);

        return true;
    }

    bool game_manager::run() noexcept {
        this->_log_manager->log_message("Running the game manager...", apis::logging::log_levels::info);

        this->_log_manager->log_message("Run the game manager.", apis::logging::log_levels::info);

        return true;
    }

    bool game_manager::shutdown() noexcept {
        this->_log_manager->log_message("Shutting down the game manager...", apis::logging::log_levels::info);

        this->_log_manager->log_message("Shut down the game manager.", apis::logging::log_levels::info);

        return true;
    }
}
