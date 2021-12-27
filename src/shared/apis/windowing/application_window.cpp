#include "application_window.h"

#include <SDL_vulkan.h>

namespace pbr::shared::apis::windowing {
    bool application_window::create(std::string_view title,
                                    uint32_t x, uint32_t y,
                                    uint32_t w, uint32_t h) noexcept {
        auto flags = SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE |
                     SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_SHOWN;

        this->_window = SDL_CreateWindow(std::string(title).c_str(),
                                         x, y,
                                         w, h,
                                         flags);

        if (!this->_window) {
            this->_log_manager->log_message("Failed to create SDL window, with error:",
                                            apis::logging::log_levels::error,
                                            "Windowing");
            this->_log_manager->log_message(SDL_GetError(),
                                            apis::logging::log_levels::error,
                                            "Windowing");
            return false;
        }

        return true;
    }

    window_size application_window::get_window_size() const noexcept {
        window_size size;

        int width {0};
        int height {0};

        SDL_Vulkan_GetDrawableSize(this->_window,
                                   &width,
                                   &height);

        size.width_in_pixels = static_cast<pixels>(width);
        size.height_in_pixels = static_cast<pixels>(height);

        return size;
    }

    void application_window::shutdown() noexcept {
        if (this->_window) {
            SDL_DestroyWindow(this->_window);
            this->_window = nullptr;
        }
    }
}
