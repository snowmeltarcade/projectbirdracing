#pragma once

namespace pbr::shared::apis::graphics {
    /// The settings the graphics engine should use
    struct performance_settings {
        /// Should prefer vsync?
        bool prefer_vsync {false};
    };
}
