#include "scene_manager.h"

namespace pbr::shared::scene {
    bool scene_manager::initialize() noexcept {
        this->_loaded_scenes.clear();

        return true;
    }

    bool scene_manager::run() noexcept {
        auto should_scenes_quit {false};

        if (this->_are_loading_new_scenes) {
            if (!this->_loading_scene->run()) {
                this->_log_manager->log_message("Failed to run loading scene.", apis::logging::log_levels::error);
                return false;
            }
        } else {
            for (auto &scene: this->_loaded_scenes) {
                if (!scene->run()) {
                    this->_log_manager->log_message("Failed to run scene.", apis::logging::log_levels::error);
                    return false;
                }

                if (scene->should_quit()) {
                    should_scenes_quit = true;
                }
            }

            // check for any new scenes to load after running the existing scenes first
            if (this->_loaded_scenes.empty() || should_scenes_quit) {
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

                this->_loading_scene = loading_scene;

                this->_are_loading_new_scenes = true;

                // the thread should have finished by now - otherwise we shouldn't be here, but just in case...
                if (this->_new_scene_loading_thread && this->_new_scene_loading_thread->joinable()) {
                    this->_new_scene_loading_thread->join();
                }

                this->_new_scene_loading_thread = {};

                // Start the loading and let it run. We won't check it, but rely on `this->_are_loading_new_scenes`
                this->_new_scene_loading_thread = std::make_unique<std::thread>([this]() {
                    auto next_scenes = this->_scene_factory->get_next_scenes(this->_loaded_scenes);

                    if (!this->queue_new_scenes(next_scenes)) {
                        this->_log_manager->log_message("Failed to load new scenes.", apis::logging::log_levels::fatal);
                    }

                    this->_are_loading_new_scenes = false;
                });
            }
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
