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
        glm::vec2 position;

        /// The scale
        glm::vec2 scale;

        /// The orientation
        glm::quat orientation;

        /// The color
        color color;
    };
}
