#include "scene_manager.h"

namespace pbr::shared::scene {
    bool scene_manager::run() noexcept {
        if (this->_are_loading_new_scenes) {
            if (!this->_loading_scene->run()) {
                this->_log_manager->log_message("Failed to run loading scene.",
                                                apis::logging::log_levels::error,
                                                "Scene");
                return false;
            }

            // scene loading happens in another thread, so check if that was successful
            // here as we can return here
            if (this->_did_loading_new_scenes_fail) {
                return false;
            }
        } else {
            auto should_scenes_quit {false};

            for (auto &scene: this->_loaded_scenes) {
                if (!scene->run()) {
                    this->_log_manager->log_message("Failed to run scene.",
                                                    apis::logging::log_levels::error,
                                                    "Scene");
                    return false;
                }

                if (scene->should_quit()) {
                    // if one scene quits, run the rest, but we all quit
                    should_scenes_quit = true;
                }
            }

            // check for any new scenes to load after running the existing scenes first
            if (!this->setup_loading_new_scenes(should_scenes_quit)) {
                return false;
            }
        }

        return true;
    }

    bool scene_manager::setup_loading_new_scenes(bool have_scenes_quit) noexcept {
        if (!this->_loaded_scenes.empty() && !have_scenes_quit) {
            return true;
        }

        // destroy any previously loaded 'loading' scene
        this->_loading_scene = {};

        this->_loading_scene = this->_scene_factory->create_scene(this->_loading_scene_type);
        if (!this->_loading_scene) {
            this->_log_manager->log_message("Failed to create scene with type: " +
                                            std::to_string(static_cast<uint32_t>(this->_loading_scene_type)),
                                            apis::logging::log_levels::error,
                                            "Scene");
            return false;
        }

        if (!this->_loading_scene->load()) {
            this->_log_manager->log_message("Failed to load loading scene.",
                                            apis::logging::log_levels::error,
                                            "Scene");
            return false;
        }

        this->_are_loading_new_scenes = true;

        // the scene loading thread should have finished by now - otherwise we shouldn't be here, but just in case...
        if (this->_new_scene_loading_thread && this->_new_scene_loading_thread->joinable()) {
            this->_new_scene_loading_thread->join();
        }

        // destroy the old thread
        this->_new_scene_loading_thread = {};

        // start the loading and let it run using `this->_are_loading_new_scenes` as an exit clause
        this->_new_scene_loading_thread = std::make_unique<std::thread>([this]() {
            auto next_scenes = this->_scene_factory->get_next_scenes(this->_loaded_scenes);

            if (!this->queue_new_scenes(next_scenes)) {
                this->_log_manager->log_message("Failed to load new scenes.",
                                                apis::logging::log_levels::error,
                                                "Scene");
                this->_did_loading_new_scenes_fail = true;
                return;
            }

            this->_are_loading_new_scenes = false;
        });

        return true;
    }

    bool scene_manager::queue_new_scenes(const std::vector<scene_types>& types) noexcept {
        if (types.empty()) {
            this->_log_manager->log_message("No scene types queued.",
                                            apis::logging::log_levels::error,
                                            "Scene");
            return false;
        }

        this->_loaded_scenes.clear();

        for (const auto& type : types) {
            auto scene = this->_scene_factory->create_scene(type);
            if (!scene) {
                this->_log_manager->log_message("Failed to create scene with type: " +
                                                std::to_string(static_cast<uint32_t>(type)),
                                                apis::logging::log_levels::error,
                                                "Scene");
                return false;
            }

            if (!scene->load()) {
                this->_log_manager->log_message("Failed to load with type: " +
                                                std::to_string(static_cast<uint32_t>(type)),
                                                apis::logging::log_levels::error,
                                                "Scene");
                return false;
            }

            this->_loaded_scenes.push_back(scene);
        }

        return true;
    }
}
