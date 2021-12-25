#include "window_surface.h"

#include <SDL_vulkan.h>

namespace pbr::shared::apis::graphics::vulkan {
    window_surface::window_surface(instance& instance,
                                   std::shared_ptr<windowing::application_window> window,
                                   std::shared_ptr<logging::ilog_manager> log_manager)
       : _instance(instance),
        _log_manager(log_manager) {
        this->_log_manager->log_message("Creating surface...", logging::log_levels::info);

        if (!SDL_Vulkan_CreateSurface(window->get_native_handle(),
                                      this->_instance.get_native_handle(),
                                      &this->_surface)) {
            auto message = std::string("Failed to create surface with error: ") + SDL_GetError();
            this->_log_manager->log_message(message, logging::log_levels::fatal);
            throw std::runtime_error(message);
        }

        this->_log_manager->log_message("Created surface.", logging::log_levels::info);
    }

    window_surface::~window_surface() {
        if (this->_surface) {
            vkDestroySurfaceKHR(this->_instance.get_native_handle(), this->_surface, nullptr);
            this->_surface = nullptr;
        }
    }
}
