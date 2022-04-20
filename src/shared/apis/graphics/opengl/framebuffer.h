#pragma once

#include "shared/types.h"

namespace pbr::shared::apis::graphics::opengl {
    /// Wraps a framebuffer
    class framebuffer final {
    public:
        /// Creates this frame buffer
        /// \param width The width
        /// \param height The height
        framebuffer(pixels width, pixels height);

        /// Destroys this frame buffer
        ~framebuffer();

        /// Binds this frame buffer
        void bind() const noexcept;

        /// Unbinds this frame buffer
        void unbind() const noexcept;

    private:
    };
}
