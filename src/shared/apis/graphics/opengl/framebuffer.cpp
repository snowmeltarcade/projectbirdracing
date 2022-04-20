#include "framebuffer.h"

#include <cassert>

namespace pbr::shared::apis::graphics::opengl {
    framebuffer::framebuffer(pixels width, pixels height) {
        assert((width > 0u));
        assert((height > 0u));
    }

    framebuffer::~framebuffer() {

    }

    void framebuffer::bind() const noexcept {

    }

    void framebuffer::unbind() const noexcept {

    }
}
