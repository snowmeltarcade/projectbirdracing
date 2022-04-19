#include "settings.h"
#include "shared/utils/strings.h"

namespace pbr::shared::data {
    std::optional<std::string> settings::get(const std::string& key) noexcept {
        auto it = this->_map.find(key);
        if (it == this->_map.end()) {
            return {};
        }

        return this->_map[key];
    }

    std::optional<int> settings::get_as_int(const std::string& key) noexcept {
        auto it = this->_map.find(key);
        if (it == this->_map.end()) {
            return {};
        }

        if (auto i = utils::to_int(this->_map[key]); i) {
            return i;
        }

        return {};
    }

    std::optional<uint32_t> settings::get_as_uint32_t(const std::string& key) noexcept {
        if (auto i = this->get_as_int(key); i) {
            return static_cast<uint32_t>(*i);
        }

        return {};
    }

    std::optional<float> settings::get_as_float(const std::string& key) noexcept {
        auto it = this->_map.find(key);
        if (it == this->_map.end()) {
            return {};
        }

        if (auto i = utils::to_float(this->_map[key]); i) {
            return i;
        }

        return {};
    }

    std::optional<bool> settings::get_as_bool(const std::string& key) noexcept {
        auto it = this->_map.find(key);
        if (it == this->_map.end()) {
            return {};
        }

        auto value = this->_map[key];

        // we're being strict with these values
        if (value == "true") {
            return true;
        } else if (value == "false") {
            return false;
        }

        return {};
    }

    std::optional<settings> settings::get_as_settings(const std::string& key) noexcept {
        auto it = this->_settings_map.find(key);
        if (it == this->_settings_map.end()) {
            return {};
        }

        return this->_settings_map[key];
    }

    std::optional<std::vector<settings>> settings::get_as_settings_array(const std::string& key) noexcept {
        auto it = this->_array.find(key);
        if (it == this->_array.end()) {
            return {};
        }

        return this->_array[key];
    }
}
