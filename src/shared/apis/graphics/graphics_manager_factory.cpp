#include <cassert>

#include "graphics_manager_factory.h"
#include "opengl/graphics_manager.h"
#include "vulkan/graphics_manager.h"

namespace pbr::shared::apis::graphics {
    const std::filesystem::path graphics_manager_factory::graphics_config_path = "graphics/config";

    std::shared_ptr<igraphics_manager> graphics_manager_factory::create(
        const std::shared_ptr<data::data_manager>& data_manager,
        const std::shared_ptr<logging::ilog_manager>& log_manager,
        const std::shared_ptr<logging::ilog_manager>& graphics_log_manager,
        const std::shared_ptr<windowing::iwindow_manager> window_manager,
        const application_information& application_information,
        const performance_settings& performance_settings,
        const std::filesystem::path& config_path) noexcept {

        assert((data_manager));
        assert((log_manager));
        assert((graphics_log_manager));
        assert((window_manager));

        auto settings = data_manager->read_settings(config_path);
        if (!settings) {
            log_manager->log_message("Failed to find graphics config: `" +
                std::string(config_path) + "`.",
                logging::log_levels::fatal);
            return {};
        }

        auto api_name = settings->get("api");

        if (api_name == "opengl") {
            auto manager = std::make_shared<opengl::graphics_manager>(graphics_log_manager,
                                                                      window_manager,
                                                                      application_information,
                                                                      performance_settings);
            return manager;
        } else if (api_name == "vulkan") {
            auto manager = std::make_shared<vulkan::graphics_manager>(graphics_log_manager,
                                                                      window_manager,
                                                                      application_information,
                                                                      performance_settings);
            return manager;
        }

        return {};
    }
}
