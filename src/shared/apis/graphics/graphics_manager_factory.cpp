#include <cassert>

#include "graphics_manager_factory.h"
#include "opengl/graphics_manager.h"
#include "vulkan/graphics_manager.h"

namespace pbr::shared::apis::graphics {
    std::shared_ptr<igraphics_manager> graphics_manager_factory::create(
        const config& config,
        const std::shared_ptr<data::data_manager>& data_manager,
        const std::shared_ptr<logging::ilog_manager>& log_manager,
        const std::shared_ptr<logging::ilog_manager>& graphics_log_manager,
        const std::shared_ptr<windowing::iwindow_manager>& window_manager,
        const application_information& application_information,
        const performance_settings& performance_settings) noexcept {

        assert((log_manager));
        assert((graphics_log_manager));
        assert((window_manager));

        switch (config.api()) {
            case apis::opengl: {
                auto manager = std::make_shared<opengl::graphics_manager>(data_manager,
                                                                          window_manager,
                                                                          graphics_log_manager,
                                                                          application_information,
                                                                          performance_settings);
                return manager;
            }
            case apis::vulkan: {
                auto manager = std::make_shared<vulkan::graphics_manager>(graphics_log_manager,
                                                                          window_manager,
                                                                          application_information,
                                                                          performance_settings);
                return manager;
            }
            default: {
                return {};
            }
        }
    }
}
