#pragma once

#include <optional>
#include <string>

namespace pbr::shared::scene {
    /// The available render systems
    enum class render_systems_data {
        /// The 2d render system
        _2d,

        /// The 3d render system
        _3d,
    };

    /// A location
    struct location_data final {
        /// The x-position
        float x { 0.0f };

        /// The y-position
        float y { 0.0f };

        /// The z-position
        std::optional<float> z;
    };

    /// A camera
    struct camera_data final {
        /// The name
        std::string name;

        /// The layer
        std::optional<int> layer;

        /// The render target's name
        std::optional<std::string> render_target_name;

        /// The render system
        render_systems_data system { render_systems_data::_3d };

        /// The position
        location_data position;

        /// The focus
        std::optional<location_data> focus;

        /// The up direction
        std::optional<location_data> up;
    };
}
