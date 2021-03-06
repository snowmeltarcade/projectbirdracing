#pragma once

#include "shared/apis/logging/ilog_manager.h"
#include "shared/data/data_manager.h"
#include "opengl_dependencies.h"

#include <memory>
#include <filesystem>
#include <SDL_opengl.h>

namespace pbr::shared::apis::graphics::opengl {
    /// Wraps an OpenGL shader
    class shader final {
    public:
        /// Creates this shader
        /// \param path The path to the shader
        /// \param data_manager The data manager
        /// \param log_manager The log manager
        shader(const std::filesystem::path& path,
               const std::shared_ptr<data::data_manager>& data_manager,
               const std::shared_ptr<logging::ilog_manager>& log_manager) {
            if (!this->create(path, data_manager, log_manager)) {
                log_manager->log_message("Failed to create shader with path: " + path.generic_string(),
                                         logging::log_levels::error,
                                         "Graphics");
            }
        }

        /// Destroys this shader
        ~shader() {
            glDeleteShader(this->_id);
            this->_id = 0;
        }

        /// Returns the native id
        /// \returns The native id
        [[nodiscard]]
        GLuint native_id() const noexcept {
            return this->_id;
        }

    private:
        /// The shader id
        GLuint _id;

        /// Creates the shader with the passed name
        /// \param path The path to the shader
        /// \param data_manager The data manager
        /// \param log_manager The log manager
        /// \returns `true` upon success, else `false`
        [[nodiscard]]
        bool create(const std::filesystem::path& path,
                    const std::shared_ptr<data::data_manager>& data_manager,
                    const std::shared_ptr<logging::ilog_manager>& log_manager) noexcept;

        /// Returns any error messages during this shader's creation
        /// \returns Any error messages during this shader's creation
        [[nodiscard]]
        std::string get_error_messages() const noexcept;
    };
}
