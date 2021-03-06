#include "window_manager.h"
#include "application_window.h"

#include <SDL.h>

namespace pbr::shared::apis::windowing {
    bool window_manager::initialize() noexcept {
        this->_log_manager->log_message("Initializing the window manager...",
                                        apis::logging::log_levels::info,
                                        "Windowing");

        if (SDL_Init(SDL_INIT_EVENTS) < 0) {
            this->_log_manager->log_message("Failed to init SDL with error:",
                                            apis::logging::log_levels::error,
                                            "Windowing");
            return false;
        }

        this->_log_manager->log_message("Initialized the window manager.",
                                        apis::logging::log_levels::info,
                                        "Windowing");
        return true;
    }

    std::shared_ptr<iconsole_window> window_manager::create_console_window() noexcept {
        return {};
    }

    std::shared_ptr<iapplication_window> window_manager::create_application_window() noexcept {
        auto default_resolution = this->_config.default_resolution();

        auto window = std::make_shared<application_window>(this->_log_manager,
                                                           this->_graphics_api,
                                                           "PBR",
                                                           default_resolution.width, default_resolution.height,
                                                           default_resolution.fullscreen);

        this->_application_windows.push_back(window);

        return window;
    }

    bool window_manager::update() noexcept {
        SDL_Event e;

        while (SDL_PollEvent(&e))
        {
            switch (e.type)
            {
                case SDL_APP_TERMINATING:
                {
                    this->_log_manager->log_message("Event: App terminating...",
                                                    apis::logging::log_levels::info,
                                                    "Windowing");

                    this->_should_quit = true;
                    break;
                }
                case SDL_QUIT:
                {
                    this->_log_manager->log_message("Event: Quit...",
                                                    apis::logging::log_levels::info,
                                                    "Windowing");

                    this->_should_quit = true;
                    break;
                }
                default:
                {
                    break;
                }
            }
        }

        return true;
    }

    bool window_manager::shutdown() noexcept {
        this->_log_manager->log_message("Shutting down the window manager...",
                                        apis::logging::log_levels::info,
                                        "Windowing");

        // remove references we know about so, hopefully, the destructors will be called
        this->_application_windows.clear();

        SDL_Quit();

        this->_log_manager->log_message("Shut down the window manager.",
                                        apis::logging::log_levels::info,
                                        "Windowing");
        return true;
    }
}
