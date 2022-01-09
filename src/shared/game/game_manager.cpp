#include "game_manager.h"

namespace pbr::shared::game {
    bool game_manager::initialize() noexcept {
        this->_log_manager->log_message("Initializing the game manager...",
                                        apis::logging::log_levels::info,
                                        "Game");

        if (!this->_graphics_manager->load_api(this->_executable_path)) {
            this->_log_manager->log_message("Failed to load the graphics API.",
                                            apis::logging::log_levels::error,
                                            "Game");
            return false;
        }

        if (!this->_window_manager->initialize()) {
            this->_log_manager->log_message("Failed to initialize window manager.",
                                            apis::logging::log_levels::error,
                                            "Game");
            return false;
        }

        this->_application_window = this->_window_manager->create_application_window();
        if (!this->_application_window) {
            this->_log_manager->log_message("Failed to create application window.",
                                            apis::logging::log_levels::error,
                                            "Game");
            return false;
        }

        if (!this->_graphics_manager->initialize()) {
            this->_log_manager->log_message("Failed to initialize graphics manager.",
                                            apis::logging::log_levels::error,
                                            "Game");
            return false;
        }

        this->_log_manager->log_message("Initialized the game manager.",
                                        apis::logging::log_levels::info,
                                        "Game");

        return true;
    }

    bool game_manager::run() noexcept {
        this->_log_manager->log_message("Running the game manager...",
                                        apis::logging::log_levels::info,
                                        "Game");

        auto now = std::chrono::system_clock::now();
        auto previous_now = now;
        auto angle = 0.0f;

        while (!this->_has_exit_been_requested) {
            if (!this->_window_manager->update()) {
                this->_log_manager->log_message("Failed to update window manager.",
                                                apis::logging::log_levels::error,
                                                "Game");
                return false;
            }

            if (this->_window_manager->should_quit()) {
                this->request_exit();
                continue;
            }

            if (!this->_scene_manager->run()) {
                this->_log_manager->log_message("Failed to run scene manager.",
                                                apis::logging::log_levels::error,
                                                "Game");
                return false;
            }

            apis::graphics::renderable_entities entities;

            auto frame_time = now - previous_now;
            auto mcs = std::chrono::duration_cast<std::chrono::microseconds>(frame_time).count();

            // 10 degrees per second
            angle += 10.0f * (mcs / 1000000.0f);

            entities.submit({
                                { 0.0f, 0.0f, 0.2f, },
                                1.0f, 1.0f,
                                { 1.0f, 1.0f, },
                                glm::angleAxis(glm::radians(angle * 0.7f + 10.0f),
                                               glm::vec3(0.0f, 0.0f, 1.0f)),
                                { 0, 255, 0, 255 },
                            });

            entities.submit({
                                { 0.2f, 0.7f, 0.1f, },
                                0.5f, 0.1f,
                                { 2.0f, 0.5f, },
                                glm::angleAxis(glm::radians(angle),
                                               glm::vec3(0.0f, 0.0f, 1.0f)),
                                { 100, 100, 255, 200 },
                            });

            entities.submit({
                                { 0.2f, 0.4f, 0.1f, },
                                0.25f, 0.2f,
                                { 1.0f, 1.0f, },
                                {},
                                { 255, 255, 255, 255 },
                            });

            entities.submit({
                                { 0.2f, 0.8f, 0.0f, },
                                0.1f, 0.2f,
                                { 3.0f, 0.5f, },
                                glm::angleAxis(glm::radians(-angle * 2.0f + 45.0f),
                                               glm::vec3(0.0f, 0.0f, 1.0f)),
                                { 255, 100, 255, 200 },
                            });

            entities.submit({
                                { 0.3f, 0.5f, 0.1f, },
                                0.4f, 0.3f,
                                { 1.0f, 1.0f, },
                                {},
                                { 0, 0, 255, 255 },
                            });

            this->_graphics_manager->submit_renderable_entities(entities);

            // this will be moved to another thread soon
            this->_graphics_manager->submit_frame_for_render();

            previous_now = now;
            now = std::chrono::system_clock::now();
        }

        this->_log_manager->log_message("Finished running the game manager.",
                                        apis::logging::log_levels::info,
                                        "Game");

        return true;
    }

    bool game_manager::shutdown() noexcept {
        this->_log_manager->log_message("Shutting down the game manager...",
                                        apis::logging::log_levels::info,
                                        "Game");

        this->_log_manager->log_message("Shut down the game manager.",
                                        apis::logging::log_levels::info,
                                        "Game");

        return true;
    }
}
