#include "shader_program.h"
#include "opengl_errors.h"

#include <cassert>

namespace pbr::shared::apis::graphics::opengl {
    std::optional<std::shared_ptr<shader_program>> shader_program::create(
        std::shared_ptr<logging::ilog_manager> log_manager,
        std::shared_ptr<shader_manager> shader_manager,
        std::initializer_list<std::string> names) noexcept {

        assert((log_manager));
        assert((shader_manager));

        if (names.size() == 0) {
            return {};
        }

        auto program = std::make_shared<shader_program>(log_manager);

        for (const auto& name : names) {
            auto shader = shader_manager->get(name);
            if (!shader) {
                log_manager->log_message("Failed to find shader with name: " + name,
                                         logging::log_levels::warning,
                                         "Graphics");
                continue;
            }

            if (!program->attach_shader(shader)) {
                log_manager->log_message("Failed to attach shader with name: " + name,
                                         logging::log_levels::warning,
                                         "Graphics");
                continue;
            }
        }

        if (!program->link()) {
            log_manager->log_message("Failed to link shader manager",
                                     logging::log_levels::warning,
                                     "Graphics");
            return {};
        }

        return program;
    }

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

    void shader_program::use() const noexcept {
        auto texture_index {0};

        for (const auto& texture : this->_textures) {
            glActiveTexture(GL_TEXTURE0 + texture_index);
            ++texture_index;

            glBindTexture(GL_TEXTURE_2D, texture);
            CHECK_OPENGL_ERROR_NO_RETURN(this->_log_manager);
        }

        glUseProgram(this->_id);
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

    void shader_program::bind_textures(const std::vector<texture_reference>& textures) noexcept {
        this->_textures.clear();

        if (textures.empty()) {
            return;
        }

        this->use();

        auto location_index {0u};

        for (const auto& texture : textures) {
            auto location = glGetUniformLocation(this->_id, texture.name.c_str());
            if (location < 0) {
                this->_log_manager->log_message("Failed to find texture with name: `" + texture.name + "`.",
                                                logging::log_levels::warning,
                                                "Graphics");
                continue;
            }

            glUniform1i(location, location_index++);

            this->_textures.emplace_back(texture.texture_id);
        }

        this->clear_use();
    }
}
