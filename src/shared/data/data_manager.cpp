#include "data_manager.h"

#include <nlohmann/json.hpp>

namespace pbr::shared::data {
    /// Builds a settings object from the passed json
    /// \param json The json to build from
    /// \returns The built settings
    settings build_settings(const nlohmann::json& json) {
        settings settings;

        for (const auto& item : json.items()) {
            auto key = item.key();
            auto value = item.value();

            if (value.is_string()) {
                settings.add(key, value.get<std::string>());
            } else if (value.is_number_unsigned()) { // check for unsigned before checking for generate integers
                settings.add(key, value.get<uint32_t>());
            } else if (value.is_number_integer()) {
                settings.add(key, value.get<int>());
            } else if (value.is_number_float()) {
                settings.add(key, value.get<float>());
            } else if (value.is_null()) {
                settings.add(key, "");
            } else if (value.is_boolean()) {
                settings.add(key, value.get<bool>());
            } else if (value.is_array()) {
                for (const auto& value_in_array : value) {
                    auto sub_settings = build_settings(value_in_array);
                    settings.add_to_array(key, sub_settings);
                }
            } else if (value.is_object()) {
                auto sub_settings = build_settings(value);
                settings.add(key, sub_settings);
            } else if (value.is_binary()) {
                // not supported...
            }
        }

        return settings;
    }

    std::optional<settings> data_manager::read_settings(const std::filesystem::path& relative_path) const noexcept {
        auto path = this->resolve_path(relative_path, {".json"});
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

    std::optional<shader_code> data_manager::read_shader_code(
        const std::filesystem::path& relative_path) const noexcept {
        auto path = this->resolve_path(relative_path, {".vert", ".frag"});
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

        auto file_extension = path->extension();

        auto shader_type = apis::graphics::shader_types::vertex;

        // we default to vertex, so no need to check for that
        if (file_extension == ".frag") {
            shader_type = apis::graphics::shader_types::fragment;
        }

        return { { *text, shader_type } };
    }

    std::optional<std::filesystem::path> data_manager::resolve_path(
        const std::filesystem::path& relative_path,
        const std::initializer_list<std::string>& expected_extensions) const noexcept {
        for (const auto& extension : expected_extensions) {
            auto path = this->_data_path / (relative_path.generic_string() + extension);

            if (std::filesystem::exists(path)) {
                return path;
            }
        }

        return {};
    }
}
