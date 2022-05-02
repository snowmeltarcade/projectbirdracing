#include "texture.h"
#include "shared/apis/graphics/opengl/opengl_errors.h"

#include <cassert>

namespace pbr::shared::apis::graphics::opengl::render_targets {
    void texture::bind() const noexcept {
        glBindFramebuffer(GL_FRAMEBUFFER, this->_framebuffer_id);

        glViewport(0u, 0u, this->_width, this->_height);
    }

    void texture::unbind() const noexcept {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void texture::clear() const noexcept {
        glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    }

    void texture::resize(pixels width, pixels height) noexcept {
        assert((width > 0u));
        assert((height > 0u));

        this->_width = width;
        this->_height = height;
    }

    bool texture::create(const std::shared_ptr<logging::ilog_manager>& log_manager) noexcept {
        glGenFramebuffers(1, &this->_framebuffer_id);
        CHECK_OPENGL_ERROR(log_manager);

        this->bind();

        glGenTextures(1, &this->_texture_id);
        CHECK_OPENGL_ERROR(log_manager);

        glBindTexture(GL_TEXTURE_2D, this->_texture_id);
        CHECK_OPENGL_ERROR(log_manager);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, this->_width, this->_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
        CHECK_OPENGL_ERROR(log_manager);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        CHECK_OPENGL_ERROR(log_manager);

//        GLuint depth_buffer_id {0};
//        glGenRenderbuffers(1, &depth_buffer_id);
//        glBindRenderbuffer(GL_RENDERBUFFER, depth_buffer_id);
//        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, this->_width, this->_height);
//        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depth_buffer_id);
//        CHECK_OPENGL_ERROR(log_manager);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, this->_texture_id, 0);
        CHECK_OPENGL_ERROR(log_manager);

//        GLenum draw_buffers[1] { GL_COLOR_ATTACHMENT0 };
//        glDrawBuffers(1, draw_buffers);
//        CHECK_OPENGL_ERROR(log_manager);

        if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            log_manager->log_message("Failed to create framebuffer.",
                                     logging::log_levels::error,
                                     "Graphics");
            return false;
        }

        this->unbind();

        glBindTexture(GL_TEXTURE_2D, 0);
        CHECK_OPENGL_ERROR(log_manager);

        return true;
    }
}
