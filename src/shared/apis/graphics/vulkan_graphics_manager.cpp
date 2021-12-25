#include "vulkan_graphics_manager.h"
#include "shared/apis/windowing/application_window.h"

#include <SDL_vulkan.h>

using namespace pbr::shared::apis;

namespace pbr::shared::apis::graphics {
    bool vulkan_graphics_manager::load_api() {
        this->_log_manager->log_message("Loading the graphics API...",
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

#ifdef REQUIRES_MOLTEN_VK
        if (SDL_Vulkan_LoadLibrary("./libMoltenVK.dylib") < 0) {
            this->_log_manager->log_message("Failed to load MoltenVK library with error:",
                                            apis::logging::log_levels::error,
                                            "Graphics");
            this->_log_manager->log_message(SDL_GetError(),
                                            apis::logging::log_levels::error,
                                            "Graphics");
            return false;
        }
#endif

        this->_log_manager->log_message("Loaded the graphics API.",
                                        apis::logging::log_levels::info,
                                        "Graphics");

        return true;
    }

    bool vulkan_graphics_manager::initialize() {
        this->_log_manager->log_message("Initializing the graphics manager...",
                                        apis::logging::log_levels::info,
                                        "Graphics");

        auto application_window = std::dynamic_pointer_cast<windowing::application_window>(
            this->_window_manager->get_main_application_window());
        assert((application_window));

        if (!this->_instance.initialize(application_window->get_native_handle(),
                                        this->_application_information)) {
            this->_log_manager->log_message("Failed to initialize Vulkan instance.",
                                            logging::log_levels::error,
                                            "Graphics");
            return false;
        }

        this->_log_manager->log_message("Initialized the graphics manager.",
                                        apis::logging::log_levels::info,
                                        "Graphics");

        return true;
    }

    bool vulkan_graphics_manager::shutdown() noexcept {
        this->_log_manager->log_message("Shutting down the graphics API...",
                                        apis::logging::log_levels::info,
                                        "Graphics");

        SDL_VideoQuit();

        this->_log_manager->log_message("Shut down the graphics API.",
                                        apis::logging::log_levels::info,
                                        "Graphics");

        return true;
    }
}
