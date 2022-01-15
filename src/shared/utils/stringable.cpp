#include "stringable.h"

namespace pbr::shared::utils {
    std::string to_string(int value) noexcept {
        return std::to_string(value);
    }

    std::string to_string(bool value) noexcept {
        return value ? "true" : "false";
    }
}
