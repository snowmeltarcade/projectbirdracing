#include "vulkan_graphics_manager.h"
#include "shared/apis/windowing/application_window.h"

using namespace pbr::shared::apis;

namespace pbr::shared::apis::graphics {
    bool vulkan_graphics_manager::initialize() {
        auto application_window = std::dynamic_pointer_cast<windowing::application_window>(
            this->_window_manager->get_main_application_window());
        assert((application_window));

        if (!this->_instance.initialize(application_window->get_native_handle(),
                                        this->_application_information)) {
            this->_log_manager->log_message("Failed to initialize Vulkan instance.", logging::log_levels::error);
            return false;
        }

        return true;
    }
}
