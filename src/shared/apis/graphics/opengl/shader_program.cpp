#include "shader_program.h"

#include <cassert>

namespace pbr::shared::apis::graphics::opengl {
    bool shader_program::attach_shader(const std::shared_ptr<shader>& shader) noexcept {
        assert((shader));

        if (this->_linked) {
            this->_log_manager->log_message("Shader program already linked. Cannot attach shader.",
                                            logging::log_levels::warning,
                                            "Graphics");
            return false;
        }

        glAttachShader(this->_id, shader->native_id());

        return true;
    }

    bool shader_program::link() noexcept {
        if (this->_linked) {
            this->_log_manager->log_message("Shader program already linked. Cannot re-link.",
                                            logging::log_levels::warning,
                                            "Graphics");
            return false;
        }

        glLinkProgram(this->_id);

        GLint linkSuccess = GL_FALSE;
        glGetProgramiv(this->_id, GL_LINK_STATUS, &linkSuccess);
        if (linkSuccess != GL_TRUE)
        {
            this->_log_manager->log_message("Failed to link shader program with error(s):\n\n" +
                                                this->get_error_messages(),
                                            logging::log_levels::error,
                                            "Graphics");
            return false;
        }

        this->_linked = true;

        return true;
    }

    std::string shader_program::get_error_messages() const noexcept {
        GLint log_length = 0;
        glGetProgramiv(this->_id, GL_INFO_LOG_LENGTH, &log_length);

        auto log = new char[log_length];

        GLint actual_log_length = 0;
        glGetProgramInfoLog(this->_id, log_length, &actual_log_length, log);

        std::string final_log;

        if (actual_log_length > 0) {
            final_log = log;
        } else {
            final_log = "No error log returned.";
        }

        delete[] log;
        log = nullptr;

        return final_log;
    }
}
