#pragma once

#include <optional>
#include <cstdint>

namespace pbr::shared::apis::graphics {
    /// The index of a camera render layer
    typedef std::optional<uint32_t> camera_layer_index;

    /// The index of a render target
    typedef std::optional<uint32_t> render_target_index;

    /// A coordinate
    typedef float coordinate;
}
