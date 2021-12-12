#include "window_manager.h"
#include "application_window.h"

#include <SDL.h>

namespace pbr::shared::apis::windowing {
    bool window_manager::initialize() noexcept {
        this->_log_manager->log_message("Initializing the window manager...", apis::logging::log_levels::info);

        if (SDL_Init(SDL_INIT_VIDEO) < 0) {
            this->_log_manager->log_message("Failed to init SDL.", apis::logging::log_levels::error);
            return false;
        }

        this->_log_manager->log_message("Initialized the window manager.", apis::logging::log_levels::info);
        return true;
    }

    std::shared_ptr<iconsole_window> window_manager::create_console_window() noexcept {
        return {};
    }

    std::shared_ptr<iapplication_window> window_manager::create_application_window() noexcept {
        auto window = std::make_shared<application_window>(this->_log_manager, "PBR", 500, 500);

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
                    this->_log_manager->log_message("Event: App terminating...", apis::logging::log_levels::info);

                    this->_should_quit = true;
                    break;
                }
                case SDL_QUIT:
                {
                    this->_log_manager->log_message("Event: Quit...", apis::logging::log_levels::info);

                    this->_should_quit = true;
                    break;
                }
                case SDL_WINDOWEVENT: {
                    switch (e.window.event) {
                        case SDL_WINDOWEVENT_SIZE_CHANGED: {
                            this->_application_windows[0]->size_changed();
                        }
                        case SDL_WINDOWEVENT_MAXIMIZED: {
                            this->_application_windows[0]->size_changed();
                        }
                        default: {
                            break;
                        }
                    }
                }
                default:
                {
                    break;
                }
            }
        }

        this->_application_windows[0]->update();

        return true;
    }

    bool window_manager::shutdown() noexcept {
        this->_log_manager->log_message("Shutting down the window manager...", apis::logging::log_levels::info);

        // remove references we know about so, hopefully, the destructors will be called
        this->_application_windows.clear();

        SDL_Quit();

        this->_log_manager->log_message("Shut down the window manager.", apis::logging::log_levels::info);
        return true;
    }
}
