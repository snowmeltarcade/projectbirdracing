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

        this->_window_surface = std::make_unique<vulkan::window_surface>(this->_instance,
                                                                         application_window,
                                                                         this->_log_manager);

        this->_physical_device = std::make_unique<vulkan::physical_device>(this->_instance,
                                                                           *this->_window_surface,
                                                                           this->_log_manager);

        this->_device = std::make_unique<vulkan::device>(this->_instance,
                                                         *this->_physical_device,
                                                         this->_log_manager);

        this->_vma = std::make_unique<vulkan::vma>(this->_instance,
                                                   *this->_physical_device,
                                                   *this->_device,
                                                   this->_log_manager);

        auto queue_indexes = this->_physical_device->get_queue_family_indexes();

        this->_graphics_queue = std::make_unique<vulkan::queue>(*this->_device,
                                                                *queue_indexes.graphics_family_index,
                                                                this->_log_manager);

        this->_present_queue = std::make_unique<vulkan::queue>(*this->_device,
                                                               *queue_indexes.present_family_index,
                                                               this->_log_manager);

        this->_log_manager->log_message("Initialized the graphics manager.",
                                        apis::logging::log_levels::info,
                                        "Graphics");

        return true;
    }

    bool vulkan_graphics_manager::shutdown() noexcept {
        this->_log_manager->log_message("Shutting down the graphics API...",
                                        apis::logging::log_levels::info,
                                        "Graphics");

        this->_vma.reset();

        this->_device.reset();

        this->_physical_device.reset();

        this->_window_surface.reset();

        SDL_VideoQuit();

        this->_log_manager->log_message("Shut down the graphics API.",
                                        apis::logging::log_levels::info,
                                        "Graphics");

        return true;
    }
}
