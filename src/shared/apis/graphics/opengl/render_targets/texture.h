#pragma once

#include "shared/apis/graphics/opengl/irender_target.h"
#include "shared/apis/graphics/opengl/opengl_dependencies.h"
#include "shared/apis/logging/ilog_manager.h"
#include "shared/types.h"

#include <memory>

namespace pbr::shared::apis::graphics::opengl::render_targets {
    /// An in-memory texture
    class texture final : public irender_target {
    public:
        /// Constructs this texture
        /// \param width The width of the texture
        /// \param height The height of the texture
        /// \param log_manager The log manager
        texture(pixels width, pixels height,
                const std::shared_ptr<logging::ilog_manager>& log_manager) {
            this->resize(width, height);

            if (!this->create(log_manager)) {
                log_manager->log_message("Failed to create texture render target.",
                                         logging::log_levels::error,
                                         "Graphics");
            }
        }

        /// Destroys this texture
        ~texture() override {
            glDeleteTextures(1, &this->_texture_id);
            glDeleteFramebuffers(1, &this->_framebuffer_id);
        }

        /// Binds this render target
        void bind() const noexcept override;

        /// Unbinds this render target
        void unbind() const noexcept override;

        /// Clears this texture
        void clear() const noexcept override;

        /// Resizes this render target
        /// \param width The width
        /// \param height The height
        void resize(pixels width, pixels height) noexcept override;

        /// Returns the backing texture id for this target
        /// \returns The texture id
        [[nodiscard]]
        GLuint texture_id() const noexcept override {
            return this->_texture_id;
        }

    private:
        /// The width
        pixels _width;

        /// The height
        pixels _height;

        /// The framebuffer id
        GLuint _framebuffer_id;

        /// The texture id
        GLuint _texture_id;

        /// Creates the framebuffer
        /// \param log_manager The log manager
        /// \returns `true` upon success, else `false`
        [[nodiscard]]
        bool create(const std::shared_ptr<logging::ilog_manager>& log_manager) noexcept;
    };
}
