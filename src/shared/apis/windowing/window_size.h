#pragma once

#include "shared/types.h"

namespace pbr::shared::apis::windowing {
    struct window_size {
        /// The width (for HDPI displays may be different from logical width)
        pixels width {0u};

        /// The height (for HDPI displays may be different from logical height)
        pixels height {0u};
    };
}
