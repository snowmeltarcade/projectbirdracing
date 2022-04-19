#include "config.h"

namespace pbr::shared::apis::windowing {
    const std::filesystem::path config::window_config_path = "windowing/config";

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
                                     "Windowing");
            return false;
        }

        this->read_resolutions(*settings);

        return true;
    }

    void config::read_resolutions(data::settings& settings) noexcept {
        auto resolutions = settings.get_as_settings("resolutions");
        if (!resolutions) {
            return;
        }

//        for (const auto& resolution : resolutions->get_as_array()) {
//
//        }
    }
}
