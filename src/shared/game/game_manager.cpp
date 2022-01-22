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

        this->_counter_set.get_counter_for_duration("fps", std::chrono::seconds(1), this->_fps);
        this->_counter_set.get_average_for_duration("average_frame_time", std::chrono::seconds(1), this->_average_frame_time);

        this->_log_manager->log_message("Initialized the game manager.",
                                        apis::logging::log_levels::info,
                                        "Game");

        return true;
    }

    bool game_manager::run() noexcept {
        this->_log_manager->log_message("Running the game manager...",
                                        apis::logging::log_levels::info,
                                        "Game");

        std::thread graphics_thread(&game_manager::run_graphics_manager,
                                    this->_graphics_manager,
                                    std::reference_wrapper(this->_has_exit_been_requested));

        while (!this->_has_exit_been_requested) {
            this->begin_frame();

            if (!this->update_frame()) {
                this->_log_manager->log_message("Failed to update frame.",
                                                apis::logging::log_levels::error,
                                                "Game");
                return false;
            }

            this->synchronize_frame();

            this->exit_frame();
        }

        if (graphics_thread.joinable()) {
            graphics_thread.join();
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

    void game_manager::request_exit() noexcept {
        this->_log_manager->log_message("Requesting exit from game manager...",
                                        apis::logging::log_levels::info,
                                        "Game");

        this->_has_exit_been_requested = true;
    }

    void game_manager::begin_frame() noexcept {
    }

    void game_manager::exit_frame() noexcept {
        auto now = std::chrono::system_clock::now();

        auto last_frame_duration = now - this->_last_frame_time;
        this->_counter_set.add_value_to_list("average_frame_time",
                                             std::chrono::duration_cast<std::chrono::milliseconds>(last_frame_duration).count());

        this->_counter_set.increment_counter("fps", 1);

        this->_last_frame_time = now;

        this->_counter_set.get_counter_for_duration("fps",
                                                    std::chrono::seconds(1),
                                                    this->_fps);

        this->_counter_set.get_average_for_duration("average_frame_time",
                                                    std::chrono::seconds(1),
                                                    this->_average_frame_time);

        this->_log_manager->log_message("FPS: " + std::to_string(this->_fps), apis::logging::log_levels::info);
        this->_log_manager->log_message("Average Frame Time: " + std::to_string(this->_average_frame_time), apis::logging::log_levels::info);
        //this->_log_manager->log_message("Frame...", apis::logging::log_levels::info);
    }

    bool game_manager::update_frame() noexcept {
        if (!this->_window_manager->update()) {
            this->_log_manager->log_message("Failed to update window manager.",
                                            apis::logging::log_levels::error,
                                            "Game");
            return false;
        }

        if (this->_window_manager->should_quit()) {
            this->request_exit();
            return true;
        }

        if (!this->_scene_manager->run()) {
            this->_log_manager->log_message("Failed to run scene manager.",
                                            apis::logging::log_levels::error,
                                            "Game");
            return false;
        }

        return true;
    }

    void game_manager::synchronize_frame() noexcept {
        apis::graphics::renderable_entities renderable_entities;
        this->_graphics_manager->submit_renderable_entities(renderable_entities);
    }

    void game_manager::run_graphics_manager(std::shared_ptr<apis::graphics::igraphics_manager> graphics_manager,
                                            std::atomic_bool& has_exit_been_requested) noexcept {
        while (!has_exit_been_requested) {
            graphics_manager->submit_frame_for_render();
        }
    }
}
