#pragma once

#include <cstdint>

namespace pbr::shared::apis::graphics {
    /// The settings the graphics engine should use
    struct performance_settings {
        /// Should prefer vsync?
        bool prefer_vsync {false};

        /// The MSAA samples to use for both color and depth images
        uint32_t msaa_samples {64};
    };
}
