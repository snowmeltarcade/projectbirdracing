#pragma once

#include "shared/apis/graphics/opengl/irender_target.h"
#include "shared/types.h"

#include <cassert>

namespace pbr::shared::apis::graphics::opengl::render_targets {
    /// The screen, or video memory
    class screen final : public irender_target {
    public:
        /// Constructs this screen
        /// \param width The width of the screen
        /// \param height The height of the screen
        screen(pixels width, pixels height) {
            this->resize(width, height);
        }

        /// Destroys this screen
        ~screen() override = default;

        /// Binds this render target
        void bind() const noexcept override;

        /// Unbinds this render target
        void unbind() const noexcept override;

        /// Clears the screen
        void clear() const noexcept override;

        /// Resizes this render target
        /// \param width The width
        /// \param height The height
        void resize(pixels width, pixels height) noexcept override;

        /// Returns the backing texture id for this target
        /// \returns The texture id
        [[nodiscard]]
        GLuint texture_id() const noexcept override {
            // TODO: get the id by reading the screens pixels...
            assert((false && "get texture id"));
            return 0;
        }

    private:
        /// The width
        pixels _width;

        /// The height
        pixels _height;
    };
}
