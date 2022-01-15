#pragma once

#include "shared/utils/stringable.h"

#include <unordered_map>
#include <string>
#include <optional>

namespace pbr::shared::data {
    /// A collection of settings. Each setting has a string key and a value. The
    /// value can be a string, an integer, a float, a boolean, another settings
    /// object or an array of one of these types. The key can be any string value,
    /// including an empty string.
    class settings {
    public:
        /// Adds a setting. If the key already exists, its value is overwritten
        /// \tparam T The type of the value to add
        /// \param key The key
        /// \param value The value
        template <utils::stringable T>
        void add(const std::string& key, T value) noexcept {
            auto s = utils::to_string(value);

            this->_map[key] = s;
        }

        /// Adds a setting. If the key already exists, its value is overwritten
        /// \param key The key
        /// \param value The value
        void add(const std::string& key, const std::string& value) noexcept {
            this->_map[key] = value;
        }

        /// Adds a setting. If the key already exists, its value is overwritten
        /// \param key The key
        /// \param value The value
        void add(const std::string& key, const settings& value) noexcept {
            this->_settings_map[key] = value;
        }

        /// Returns a setting as a string
        /// \param key The key
        /// \returns The value, else empty if the key is not present
        std::optional<std::string> get(const std::string& key) noexcept;

        /// Returns a setting as an int
        /// \param key The key
        /// \returns The value, else empty if the key is not present
        std::optional<int> get_as_int(const std::string& key) noexcept;

        /// Returns a settings object
        /// \param key The key
        /// \returns The value, else empty if the key is not present
        std::optional<settings> get_as_settings(const std::string& key) noexcept;

        /// Equality operator
        bool operator ==(const settings&) const = default;

    private:
        /// Stores the settings that can be stringified
        std::unordered_map<std::string, std::string> _map;

        /// Stores other settings objects
        std::unordered_map<std::string, settings> _settings_map;
    };
}
