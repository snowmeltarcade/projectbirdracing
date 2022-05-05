#pragma once

#include "shader.h"
#include "shader_manager.h"
#include "shared/apis/logging/ilog_manager.h"

#include <memory>
#include <SDL_opengl.h>
#include <glm/mat4x4.hpp>

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

        /// Creates a shader program from the passed shader names
        /// \param log_manager The log manager
        /// \param shader_manager The shader manager
        /// \param names The names of the shaders to attach
        /// \returns The created shader program, else empty on error. If there are
        /// no names, empty is also returned.
        static std::optional<std::shared_ptr<shader_program>> create(std::shared_ptr<logging::ilog_manager> log_manager,
                                                                     std::shared_ptr<shader_manager> shader_manager,
                                                                     std::initializer_list<std::string> names) noexcept;

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
        void use() const noexcept;

        /// Resets the used program
        void clear_use() const noexcept {
            glUseProgram(0);
        }

        /// Returns the native id
        [[nodiscard]]
        GLuint id() const noexcept {
            return this->_id;
        }

        /// Binds a texture to a uniform name in this shader
        struct texture_reference {
            /// The uniform name
            std::string name;

            /// The texture id
            /// TODO: This needs to be a texture resource reference (whatever this proves to be)
            /// when the resource manager is read for this
            GLuint texture_id;
        };

        /// Binds the texture names to their locations. The location is set to
        /// the index of the name in the vector. This clears any previously set
        /// textures.
        /// \param textures The texture references
        void bind_textures(const std::vector<texture_reference>& textures) noexcept;

        /// Sets a uniform with the passed name
        /// \param name The name of the uniform
        /// \param value The value to set
        /// \returns `true` upon success, else `false` if the uniform is not found
        [[nodiscard]]
        bool set_uniform(const std::string& name, int value) const noexcept;

        /// Sets a uniform with the passed name
        /// \param name The name of the uniform
        /// \param value The value to set
        /// \returns `true` upon success, else `false` if the uniform is not found
        [[nodiscard]]
        bool set_uniform(const std::string& name, const glm::mat4& value) const noexcept;

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

        /// The textures to bind when using this shader
        std::vector<GLuint> _textures;
    };
}
