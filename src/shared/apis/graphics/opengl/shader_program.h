#pragma once

#include "shader.h"
#include "shared/apis/logging/ilog_manager.h"

#include <memory>
#include <SDL_opengl.h>

namespace pbr::shared::apis::graphics::opengl {
    /// A set of shaders bound together to form a program
    class shader_program final {
    public:
        /// Creates this shader program
        /// \param log_manager The log manager
        shader_program(std::shared_ptr<logging::ilog_manager> log_manager)
            : _log_manager(log_manager) {
            this->_id = glCreateProgram();
        }

        /// Destroys this shader program
        ~shader_program() {
            glDeleteProgram(this->_id);
            this->_id = 0;
        }

        /// Attaches a shader to this program
        /// \param shader The shader to attach
        /// \returns `true` upon success, else `false` if this shader is already linked
        [[nodiscard]]
        bool attach_shader(const std::shared_ptr<shader>& shader) noexcept;

        /// Links this program
        /// \returns `true` upon success, else `false` if this shader is already linked
        [[nodiscard]]
        bool link() noexcept;

        /// Uses this program
        void use() const noexcept {
            glUseProgram(this->_id);
        }

        /// Resets the used program
        void clear_use() const noexcept {
            glUseProgram(0);
        }

    private:
        /// The log manager
        std::shared_ptr<logging::ilog_manager> _log_manager;

        /// The program id
        GLuint _id {0u};

        /// Is this program already linked?
        bool _linked {false};

        /// Returns any error messages when linking this shader program
        /// \returns Any error messages when linking this shader program
        [[nodiscard]]
        std::string get_error_messages() const noexcept;
    };
}
