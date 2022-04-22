#include "shader.h"

namespace pbr::shared::apis::graphics::opengl {
    bool shader::create(const std::filesystem::path& path,
                        const std::shared_ptr<data::data_manager>& data_manager,
                        const std::shared_ptr<logging::ilog_manager>& log_manager) noexcept {
        auto shader_code = data_manager->read_shader_code(path);
        if (!shader_code) {
            log_manager->log_message("Failed to load shader at path: " + path.generic_string(),
                                     logging::log_levels::error,
                                     "Graphics");
            return false;
        }

        auto shader_type = shader_code->type == shader_types::vertex ? GL_VERTEX_SHADER : GL_FRAGMENT_SHADER;
        this->_id = glCreateShader(shader_type);

        auto shader_code_content = shader_code->code.c_str();
        glShaderSource(this->_id, 1, &shader_code_content, nullptr);

        glCompileShader(this->_id);

        auto compiled_status = GL_FALSE;
        glGetShaderiv(this->_id, GL_COMPILE_STATUS, &compiled_status);
        if (compiled_status != GL_TRUE) {
            log_manager->log_message("Failed to compile shader with source:\n\n" + shader_code->code +
                                     "\n\nwith error(s):\n\n" + this->get_error_messages(),
                                     logging::log_levels::error,
                                     "Graphics");
            return false;
        }

        return true;
    }

    std::string shader::get_error_messages() const noexcept {
        GLint log_length = 0;
        glGetShaderiv(this->_id, GL_INFO_LOG_LENGTH, &log_length);

        auto log = new char[log_length];

        GLint actual_log_length = 0;
        glGetShaderInfoLog(this->_id, log_length, &actual_log_length, log);

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