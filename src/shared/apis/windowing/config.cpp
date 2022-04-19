#include "config.h"

namespace pbr::shared::apis::windowing {
    const std::filesystem::path config::window_config_path = "window/config";

    bool config::load(const std::shared_ptr<data::data_manager>& data_manager,
                      const std::shared_ptr<logging::ilog_manager>& log_manager,
                      const std::filesystem::path& config_path) noexcept {

        assert((data_manager));
        assert((log_manager));

        auto settings = data_manager->read_settings(config_path);
        if (!settings) {
            log_manager->log_message("Failed to read window config settings at path: " +
                                     config_path.generic_string(),
                                     logging::log_levels::fatal,
                                     "Window");
            return false;
        }

        return true;
    }
}
