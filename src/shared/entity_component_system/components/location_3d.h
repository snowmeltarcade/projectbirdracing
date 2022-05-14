#pragma once

#include "shared/apis/graphics/types.h"

namespace pbr::shared::entity_component_system::components {
    /// A 3d location
    struct location_3d final {
        // The x-position
        apis::graphics::coordinate x;

        // The y-position
        apis::graphics::coordinate y;

        // The z-position
        apis::graphics::coordinate z;
    };
}
