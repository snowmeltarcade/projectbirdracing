#include "splash_screen_scene.h"

namespace pbr::shared::scene::scenes {
    bool splash_screen_scene::load() noexcept {
        this->_log_manager->log_message("Loading the splash screen scene...",
                                        apis::logging::log_levels::info,
                                        "Scene");
        return true;
    }

    bool splash_screen_scene::run() noexcept {
        //this->_log_manager->log_message("Running the splash screen scene...",
        // apis::logging::log_levels::info,
        // "Scene");
        return true;
    }
}
