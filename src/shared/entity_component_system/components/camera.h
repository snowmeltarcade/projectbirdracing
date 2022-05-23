#pragma once

#include "shared/apis/graphics/types.h"
#include "shared/apis/graphics/render_systems.h"

#include <string>

namespace pbr::shared::entity_component_system::components {
    /// A camera that can render to a render target
    struct camera final {
        /// The name of this camera
        std::string name;

        /// The render target index
        apis::graphics::render_target_index render_target_index;

        /// The render system
        apis::graphics::render_systems render_system {apis::graphics::render_systems::_3d};

        /// The camera layer index
        apis::graphics::camera_layer_index layer_index;

        /// Operator ==
        /// \param other The other to compare against
        bool operator ==(const camera& other) const noexcept = default;

        /// Operator !=
        /// \param other The other to compare against
        bool operator !=(const camera& other) const noexcept = default;
    };
}
