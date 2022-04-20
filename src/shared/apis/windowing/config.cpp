#include "config.h"

namespace pbr::shared::apis::windowing {
    const std::filesystem::path config::window_config_path = "windowing/config";

    /// Returns the value represented by the passed key, or the default value if the key is not found
    /// \param settings The settings to read from
    /// \param key The key
    /// \param default_value The default value
    /// \returns The value represented by the key or the default value if the key is not found
    int get_value_or_default(data::settings& settings, const std::string& key, int default_value) {
        if (auto value = settings.get_as_int(key); value) {
            return *value;
        }

        return default_value;
    }

    /// Returns the value represented by the passed key, or the default value if the key is not found
    /// \param settings The settings to read from
    /// \param key The key
    /// \param default_value The default value
    /// \returns The value represented by the key or the default value if the key is not found
    bool get_value_or_default(data::settings& settings, const std::string& key, bool default_value) {
        if (auto value = settings.get_as_bool(key); value) {
            return *value;
        }

        return default_value;
    }

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
        auto resolutions = settings.get_as_settings_array("resolutions");
        if (!resolutions) {
            return;
        }

        this->_resolutions.clear();
        this->_default_resolution_index = 0;

        auto setting_index {0u};
        for (auto& resolution_setting : *resolutions) {
            resolution resolution;

            resolution.width = get_value_or_default(resolution_setting, "width", 0);
            resolution.height = get_value_or_default(resolution_setting, "height", 0);
            resolution.fullscreen = get_value_or_default(resolution_setting, "fullscreen", false);

            auto is_default = get_value_or_default(resolution_setting, "default", false);

            // if there is more than one resolution marked as default, we only care about the first one
            if (is_default && this->_default_resolution_index == 0) {
                this->_default_resolution_index = setting_index;
            }

            this->_resolutions.push_back(resolution);
            ++setting_index;
        }
    }
}
