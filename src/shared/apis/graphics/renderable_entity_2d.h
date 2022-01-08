#pragma once

#include <cstdint>
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include "color.h"

namespace pbr::shared::apis::graphics {
    /// A 2d entity to be rendered by the 2d render system
    struct renderable_entity_2d {
        /// The position - 0,0 is top left
        /// +Z is further back
        glm::vec3 position;

        /// The width - 1.0f is the full width of the screen
        float width {1.0f};

        /// The height - 1.0f is the full height of the screen
        float height {1.0f};

        /// The scale
        glm::vec2 scale;

        /// The orientation
        glm::quat orientation;

        /// The color
        color color;
    };
}
