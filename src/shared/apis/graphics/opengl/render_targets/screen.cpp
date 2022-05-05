#include "screen.h"
#include "shared/apis/graphics/opengl/opengl_dependencies.h"

#include <cassert>

namespace pbr::shared::apis::graphics::opengl::render_targets {
    void screen::bind() const noexcept {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        glViewport(0u, 0u, this->_width, this->_height);
    }

    void screen::unbind() const noexcept {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void screen::clear() const noexcept {
        glClearColor(1.0f, 0.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    }

    void screen::resize(pixels width, pixels height) noexcept {
        assert((width > 0u));
        assert((height > 0u));

        this->_width = width;
        this->_height = height;
    }
}
