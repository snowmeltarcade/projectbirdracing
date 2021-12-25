#include "vulkan_graphics_manager.h"
#include "shared/apis/windowing/application_window.h"

#include <cstdlib>
#include <SDL_vulkan.h>

using namespace pbr::shared::apis;

namespace pbr::shared::apis::graphics {
    bool vulkan_graphics_manager::load_api(const std::filesystem::path& executable_path) {
        this->_log_manager->log_message("Loading the graphics API...",
                                        apis::logging::log_levels::info,
                                        "Graphics");

        this->set_environment_variables(executable_path);

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
        auto lib_path = executable_path / MOLTENVK_LIB_FILENAME;
        if (SDL_Vulkan_LoadLibrary(lib_path.c_str()) < 0) {
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

    void vulkan_graphics_manager::set_environment_variables(const std::filesystem::path& executable_path) const noexcept {
#if defined(DEBUG) && defined(REQUIRES_MOLTEN_VK)
        this->_log_manager->log_message("Setting `VK_LAYER_PATH` to " + std::string(VK_LAYER_PATH),
                                        apis::logging::log_levels::info,
                                        "Graphics");
        setenv("VK_LAYER_PATH", VK_LAYER_PATH, 0);
#endif

        auto icd_path = executable_path / VK_ICD_FILENAMES;
        this->_log_manager->log_message("Setting `VK_ICD_FILENAMES` to " + std::string(icd_path),
                                        apis::logging::log_levels::info,
                                        "Graphics");
        setenv("VK_ICD_FILENAMES", icd_path.c_str(), 0);
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
