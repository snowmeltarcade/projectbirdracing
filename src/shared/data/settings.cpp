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

    std::optional<settings> settings::get_as_settings(const std::string& key) noexcept {
        auto it = this->_settings_map.find(key);
        if (it == this->_settings_map.end()) {
            return {};
        }

        return this->_settings_map[key];
    }
}
