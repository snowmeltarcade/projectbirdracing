#pragma once

#include "shared/types.h"

#include <optional>

namespace pbr::shared::apis::graphics::opengl {
    /// A destination for a camera to render to
    class irender_target {
    public:
        /// Constructs this render target
        irender_target() = default;

        /// Destroys this render target
        virtual ~irender_target() = default;

        /// Binds this render target
        virtual void bind() const noexcept = 0;

        /// Unbinds this render target
        virtual void unbind() const noexcept = 0;

        /// Resizes this render target
        /// \param width The width
        /// \param height The height
        virtual void resize(pixels width, pixels height) noexcept = 0;
    };
}
