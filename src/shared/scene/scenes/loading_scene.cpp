#include "loading_scene.h"

namespace pbr::shared::scene::scenes {
    bool loading_scene::load() noexcept {
        this->_log_manager->log_message("Loading the loading scene...",
                                        apis::logging::log_levels::info,
                                        "Scene");
        return true;
    }

    bool loading_scene::run() noexcept {
        //this->_log_manager->log_message("Running the loading scene...",
        // apis::logging::log_levels::info,
        // "Scene");
        return true;
    }
}
