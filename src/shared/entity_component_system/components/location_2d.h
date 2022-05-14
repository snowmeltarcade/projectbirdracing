#pragma once

#include "shared/apis/graphics/types.h"

namespace pbr::shared::entity_component_system::components {
    /// A 2d location
    struct location_2d final {
        // The x-position. 0.0f is left
        apis::graphics::coordinate x;

        // The y-position. 0.0f is top
        apis::graphics::coordinate y;
    };
}
