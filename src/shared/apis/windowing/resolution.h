#pragma once

#include "shared/types.h"

namespace pbr::shared::apis::windowing {
    /// A screen resolution
    struct resolution final {
        /// The width
        pixels width {0};

        /// The height
        pixels height {0};

        /// Is full screen?
        bool fullscreen {false};
    };
}
