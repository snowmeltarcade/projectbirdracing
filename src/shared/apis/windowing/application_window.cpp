#include "application_window.h"

namespace pbr::shared::apis::windowing {
    bool application_window::create(std::string_view title,
                                    uint32_t x, uint32_t y,
                                    uint32_t w, uint32_t h) noexcept {
        auto flags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE |
                     SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_SHOWN;

        this->_window = SDL_CreateWindow(std::string(title).c_str(),
                                         x, y,
                                         w, h,
                                         flags);

        if (!this->_window) {
            this->_log_manager->log_message("Failed to create SDL window.", apis::logging::log_levels::error);
            return false;
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