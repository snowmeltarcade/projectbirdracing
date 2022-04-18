#include "graphics_manager.h"
#include "shared/apis/windowing/application_window.h"

namespace pbr::shared::apis::graphics::opengl {
    bool graphics_manager::load_api(const std::filesystem::path&) noexcept {
        this->_log_manager->log_message("Loading the OpenGL graphics API...",
                                        apis::logging::log_levels::info,
                                        "Graphics");

        if (SDL_VideoInit(nullptr) < 0) {
            this->_log_manager->log_message("Failed to init SDL with error:",
                                            apis::logging::log_levels::error,
                                            "Graphics");
            this->_log_manager->log_message(SDL_GetError(),
                                            apis::logging::log_levels::error,
                                            "Graphics");
            return false;
        }

        this->_log_manager->log_message("Loaded the graphics API.",
                                        apis::logging::log_levels::info,
                                        "Graphics");

        return true;
    }

    bool graphics_manager::initialize() noexcept {
        this->_log_manager->log_message("Initializing the graphics manager...",
                                        apis::logging::log_levels::info,
                                        "Graphics");

//        auto application_window = std::dynamic_pointer_cast<windowing::application_window>(
//            this->_window_manager->get_main_application_window());
//        assert((application_window));

        this->_log_manager->log_message("Initialized the graphics manager.",
                                        apis::logging::log_levels::info,
                                        "Graphics");

        return true;
    }

    bool graphics_manager::refresh_resources() noexcept {
        return true;
    }

    bool graphics_manager::shutdown() noexcept {
        this->_log_manager->log_message("Shutting down the graphics API...",
                                        apis::logging::log_levels::info,
                                        "Graphics");

        SDL_VideoQuit();

        this->_log_manager->log_message("Shut down the graphics API.",
                                        apis::logging::log_levels::info,
                                        "Graphics");

        return true;
    }

    void graphics_manager::submit_renderable_entities(renderable_entities renderable_entities) noexcept {
        // OpenGL is single threaded, so no need for any mutexes here
        this->_renderable_entities = renderable_entities;
    }

    void graphics_manager::submit_frame_for_render() noexcept {
    }
}
