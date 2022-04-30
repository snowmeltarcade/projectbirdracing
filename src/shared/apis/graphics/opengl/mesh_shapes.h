#pragma once

#include "mesh.h"

#include <memory>

namespace pbr::shared::apis::graphics::opengl {
    /// Creates a rectangle mesh
    /// \param top The position of the top border
    /// \param left The position of the left border
    /// \param z The z position
    /// \param width The width
    /// \param height The height
    /// \param log_manager The log manager
    /// \returns The created mesh
    std::shared_ptr<mesh> create_rectangle(float top, float left, float z,
                                           float width, float height,
                                           const std::shared_ptr<logging::ilog_manager>& log_manager) noexcept;
}
