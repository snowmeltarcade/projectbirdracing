#include "world_generation_scene.h"

namespace pbr::shared::scene::scenes {
    bool world_generation_scene::load() noexcept {
        this->_log_manager->log_message("Loading the world generation scene...", apis::logging::log_levels::info);
        return true;
    }

    bool world_generation_scene::run() noexcept {
        //this->_log_manager->log_message("Running the world generation scene...", apis::logging::log_levels::info);
        return true;
    }
}
