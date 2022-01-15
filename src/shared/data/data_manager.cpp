#include "data_manager.h"

#include <nlohmann/json.hpp>

namespace pbr::shared::data {
    /// Builds a settings object from the passed json
    /// \param json The json to build from
    /// \returns The built settings
    settings build_settings(const nlohmann::json&) {
        return {};
    }

    std::optional<settings> data_manager::read_settings(const std::filesystem::path& relative_path) const noexcept {
        auto path = this->resolve_path(relative_path);
        if (!path) {
            this->_log_manager->log_message("Failed to find path: " + relative_path.generic_string(),
                                            apis::logging::log_levels::error,
                                            "Data Manager");
            return {};
        }

        auto uri = utils::build_uri("file:///" + path->generic_string());
        if (!uri) {
            this->_log_manager->log_message("Failed to build uri for path: " + relative_path.generic_string(),
                                            apis::logging::log_levels::error,
                                            "Data Manager");
            return {};
        }

        auto text = this->_file_manager->read_file_text(*uri);
        if (!text) {
            this->_log_manager->log_message("Failed to get text for path: " + relative_path.generic_string(),
                                            apis::logging::log_levels::error,
                                            "Data Manager");
            return {};
        }

        try {
            auto json = nlohmann::json::parse(*text);

            auto settings = build_settings(json);
            return settings;
        }
        catch (const std::exception& ex) {
            this->_log_manager->log_message("Failed to load JSON for path: " + relative_path.generic_string() +
                                            " with error: " + ex.what(),
                                            apis::logging::log_levels::error,
                                            "Data Manager");
            return {};
        }
    }

    std::optional<std::filesystem::path> data_manager::resolve_path(
        const std::filesystem::path& relative_path) const noexcept {
        // we're only supporting json at the moment
        auto path = this->_data_path / (relative_path.generic_string() + ".json");

        if (std::filesystem::exists(path)) {
            return path;
        }

        return {};
    }
}
