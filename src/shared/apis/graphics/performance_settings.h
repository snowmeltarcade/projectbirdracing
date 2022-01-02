#pragma once

#include <cstdint>

namespace pbr::shared::apis::graphics {
    /// The settings the graphics engine should use
    struct performance_settings {
        /// Should prefer vsync?
        bool prefer_vsync {false};

        /// The MSAA samples to use for both color and depth images
        uint32_t msaa_samples {64};

        /// The maximum number of frames that can be processed at any one time by the GPU
        uint32_t max_frames_in_flight {2u};
    };
}
