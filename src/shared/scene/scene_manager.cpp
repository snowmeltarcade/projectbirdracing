#include "scene_manager.h"

namespace pbr::shared::scene {
    bool scene_manager::initialize() noexcept {
        this->_loaded_scenes.clear();

        auto loading_scene = this->_scene_factory->create_scene(this->_loading_scene_type);
        if (!loading_scene) {
            this->_log_manager->log_message("Failed to create scene with type: " +
                                                std::to_string(static_cast<uint32_t>(this->_loading_scene_type)),
                                            apis::logging::log_levels::error);
            return false;
        }

        if (!loading_scene->load()) {
            this->_log_manager->log_message("Failed to load loading scene.", apis::logging::log_levels::error);
            return false;
        }

        this->_loaded_scenes.push_back(loading_scene);

        return true;
    }

    bool scene_manager::run() noexcept {
        // if no loaded scenes || one of the current scenes should exit
            // unload current scenes (.clear())
            // get next scenes
            // load loading scene
            // start loading next scenes
            // set is loading = true
        // if is loading
            // run loading scene
            // have next scenes loaded?
                // unload loading scene
                // set next scenes to run
        // else
            // run next scenes

        for (auto& scene : this->_loaded_scenes) {
            if (!scene->run()) {
                this->_log_manager->log_message("Failed to run scene.", apis::logging::log_levels::error);
                return false;
            }
        }

        return true;
    }

    bool scene_manager::queue_new_scenes(const std::vector<scene_types>& types) noexcept {
        if (types.empty()) {
            this->_log_manager->log_message("No scene types queued.", apis::logging::log_levels::error);
            return false;
        }

        this->_loaded_scenes.clear();

        for (const auto& type : types) {
            auto scene = this->_scene_factory->create_scene(type);
            if (!scene) {
                this->_log_manager->log_message("Failed to create scene with type: " +
                                                std::to_string(static_cast<uint32_t>(type)),
                                                apis::logging::log_levels::error);
                return false;
            }

            if (!scene->load()) {
                this->_log_manager->log_message("Failed to load with type: " +
                                                std::to_string(static_cast<uint32_t>(type)),
                                                apis::logging::log_levels::error);
                return false;
            }

            this->_loaded_scenes.push_back(scene);
        }

        return true;
    }
}
