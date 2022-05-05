#pragma once

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

namespace pbr::shared::apis::graphics::opengl {
    /// A vertex for a mesh
    struct vertex final {
        /// The position (XYZ)
        glm::vec3 position;

        /// The color (RGBA)
        glm::vec4 color;

        /// The texture coordinates (UV)
        glm::vec2 texture_coords;
    };
}
