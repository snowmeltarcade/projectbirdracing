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

//        std::thread graphics_thread(&game_manager::run_graphics_manager,
//                                    this->_graphics_manager,
//                                    this->_has_exit_been_requested,
//                                    this->_frame_synchronize_semaphore,
//                                    this->_graphics_synchronize_semaphore);
        std::thread graphics_thread(&game_manager::f);

        while (!this->_has_exit_been_requested) {
            this->begin_frame();

            if (!this->update_frame()) {
                this->_log_manager->log_message("Failed to update frame.",
                                                apis::logging::log_levels::error,
                                                "Game");
                return false;
            }

            this->enter_synchronize_frame();

            this->synchronize_frame();

            this->exit_synchronize_frame();
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
        // increase frame count
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

    void game_manager::enter_synchronize_frame() noexcept {
        // wait for the graphics thread to be done with the graphics manager
        this->_graphics_synchronize_semaphore.acquire();
    }

    void game_manager::synchronize_frame() noexcept {
        // submit renderable entities
        // ...
        // this->_graphics_manager->submit_renderable_entities()
    }

    void game_manager::exit_synchronize_frame() noexcept {
        // let the other game loop threads know we are done with their stuff
        this->_frame_synchronize_semaphore.release();
    }

    void game_manager::run_graphics_manager(std::shared_ptr<apis::graphics::igraphics_manager> graphics_manager,
                                            std::atomic_bool& has_exit_been_requested,
                                            std::binary_semaphore& frame_synchronize_semaphore,
                                            std::binary_semaphore& graphics_synchronize_semaphore) noexcept {
        while (!has_exit_been_requested) {
            // wait for the logic thread to be finished with the graphics manager
            frame_synchronize_semaphore.acquire();

            graphics_manager->submit_frame_for_render();

            // tell the logic thread we are done using the graphics manager for now
            graphics_synchronize_semaphore.release();
        }
    }
}
