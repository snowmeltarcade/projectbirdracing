#pragma once

#include "types.h"

namespace pbr::shared::apis::windowing {
    struct window_size {
        /// The width in pixels (for HDPI displays may be different from logical width)
        pixels width_in_pixels {0u};

        /// The height in pixels (for HDPI displays may be different from logical height)
        pixels height_in_pixels {0u};
    };
}
