#include "application_window.h"
#include "shared/apis/graphics/opengl/opengl_errors.h"

#include <cassert>
#include <SDL_opengl.h>
#include <SDL_vulkan.h>

using namespace std::string_literals;

namespace pbr::shared::apis::windowing {
    /// Returns the window flag for the passed api
    /// \param api The graphics api
    /// \returns The window flag for the passed api
    int api_window_flag(graphics::apis api) {
        switch (api) {
            case graphics::apis::opengl: {
                return SDL_WINDOW_OPENGL;
            }
            case graphics::apis::vulkan: {
                return SDL_WINDOW_VULKAN;
            }
            default: {
                assert(("Unknown graphics api."));
            }
        }
    }

    bool application_window::create(std::string_view title,
                                    pixels x, pixels y,
                                    pixels width, pixels height,
                                    bool fullscreen) noexcept {
        auto flags = SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_SHOWN;

        flags |= api_window_flag(this->_graphics_api);

        this->_window = SDL_CreateWindow(std::string(title).c_str(),
                                         x, y,
                                         width, height,
                                         flags);

        if (!this->_window) {
            this->_log_manager->log_message("Failed to create SDL window, with error: "s + SDL_GetError(),
                                            apis::logging::log_levels::error,
                                            "Windowing");
            return false;
        }

        if (!this->set_size(width, height, fullscreen)) {
            this->_log_manager->log_message("Failed to set window size, with error: "s + SDL_GetError(),
                                            apis::logging::log_levels::error,
                                            "Windowing");
            return false;
        }

        return true;
    }

    window_size application_window::get_size() const noexcept {
        window_size size;

        int width {0};
        int height {0};

        switch (this->_graphics_api) {
            case graphics::apis::opengl: {
                SDL_GL_GetDrawableSize(this->_window,
                                       &width,
                                       &height);
            }
            case graphics::apis::vulkan: {
                SDL_Vulkan_GetDrawableSize(this->_window,
                                           &width,
                                           &height);
            }
            default: {
                assert(("Unknown graphics api."));
            }
        }

        size.width_in_pixels = static_cast<pixels>(width);
        size.height_in_pixels = static_cast<pixels>(height);

        return size;
    }

    bool application_window::set_size(pixels width, pixels height, bool fullscreen) noexcept {
        if (fullscreen) {
            SDL_DisplayMode display_mode {
                SDL_PIXELFORMAT_RGBA32,
                static_cast<int>(width),
                static_cast<int>(height),
                0,
                0,
            };

            if (SDL_SetWindowDisplayMode(this->_window, &display_mode) != 0) {
                this->_log_manager->log_message("Failed to set window display mode, with error: "s + SDL_GetError(),
                                                logging::log_levels::error,
                                                "Windowing");
                return false;
            }

            if (SDL_SetWindowFullscreen(this->_window, SDL_WINDOW_FULLSCREEN) != 0) {
                this->_log_manager->log_message("Failed to set window to full screen, with error: "s + SDL_GetError(),
                                                logging::log_levels::error,
                                                "Windowing");
                return false;
            }
        } else {
            SDL_SetWindowSize(this->_window, static_cast<int>(width), static_cast<int>(height));

            if (SDL_SetWindowFullscreen(this->_window, 0) != 0) {
                this->_log_manager->log_message("Failed to set window to not full screen, with error: "s + SDL_GetError(),
                                                logging::log_levels::error,
                                                "Windowing");
                return false;
            }
        }

        return true;
    }

    void application_window::shutdown() noexcept {
        if (this->_window) {
            SDL_DestroyWindow(this->_window);
            this->_window = nullptr;
        }
    }
}
