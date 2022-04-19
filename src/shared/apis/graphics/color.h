#pragma once

#include <cstdint>

namespace pbr::shared::apis::graphics {
    /// An RGBA color
    struct color {
        /// Red
        uint8_t r {0u};

        /// Green
        uint8_t g {0u};

        /// Blue
        uint8_t b {0u};

        /// Alpha
        uint8_t a {255u};
    };

    namespace colors {
        /// Black
        constexpr color black { 0, 0, 0, 0 };
    }
}
