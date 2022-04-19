#include "config.h"

namespace pbr::shared::apis::graphics {
    const std::filesystem::path config::graphics_config_path = "graphics/config";

    /// Reads the api from the passed settings
    /// \param The settings
    /// \returns The graphics api if found, else empty
    std::optional<apis> read_api(data::settings& settings) {
        auto api_name = settings.get("api");

        if (api_name == "opengl") {
            return apis::opengl;
        } else if (api_name == "vulkan") {
            return apis::vulkan;
        } else {
            return {};
        }
    }

    bool config::load(const std::shared_ptr<data::data_manager>& data_manager,
                      const std::shared_ptr<logging::ilog_manager>& log_manager,
                      const std::filesystem::path& config_path) noexcept {

        assert((data_manager));
        assert((log_manager));

        auto settings = data_manager->read_settings(config_path);
        if (!settings) {
            log_manager->log_message("Failed to read graphics config settings at path: " +
                                     config_path.generic_string(),
                                     logging::log_levels::fatal,
                                     "Graphics");
            return false;
        }

        if (auto api = read_api(*settings); !api) {
            log_manager->log_message("Failed to read graphics api from the graphics config.",
                                     logging::log_levels::fatal,
                                     "Graphics");
            return false;
        } else {
            this->_api = *api;
        }

        return true;
    }
}
