#include "screen.h"

#include <cassert>
#include <GL/glew.h>

namespace pbr::shared::apis::graphics::opengl::render_targets {
    void screen::bind() const noexcept {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        glViewport(0u, 0u, this->_width, this->_height);
    }

    void screen::unbind() const noexcept {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void screen::resize(pixels width, pixels height) noexcept {
        assert((width > 0u));
        assert((height > 0u));

        this->_width = width;
        this->_height = height;
    }
}
