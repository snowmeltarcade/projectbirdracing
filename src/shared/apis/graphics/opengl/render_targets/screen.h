#pragma once

#include "shared/apis/graphics/opengl/irender_target.h"
#include "shared/types.h"

namespace pbr::shared::apis::graphics::opengl::render_targets {
    /// The screen, or video memory
    class screen final : public irender_target {
    public:
        /// Constructs this screen
        screen() = default;

        /// Destroys this screen
        ~screen() override = default;

        /// Binds this render target
        void bind() const noexcept override;

        /// Unbinds this render target
        void unbind() const noexcept override;

        /// Submits the rendered content to the screen
        void submit() const noexcept override;

        /// Resizes this render target
        /// \param width The width
        /// \param height The height
        void resize(pixels width, pixels height) noexcept override;

    private:
        /// The width
        pixels _width;

        /// The height
        pixels _height;
    };
}
