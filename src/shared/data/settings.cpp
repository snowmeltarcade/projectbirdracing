#include "settings.h"

namespace pbr::shared::data {
    std::optional<std::string> settings::get(const std::string& key) noexcept {
        auto it = this->_map.find(key);
        if (it == this->_map.end()) {
            return {};
        }

        return this->_map[key];
    }
}
