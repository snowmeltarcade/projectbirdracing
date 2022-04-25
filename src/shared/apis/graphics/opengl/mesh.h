#pragma once

#include "shared/apis/logging/ilog_manager.h"
#include "opengl_dependencies.h"

#include <memory>
#include <vector>
#include <cassert>

namespace pbr::shared::apis::graphics::opengl {
    /// A mesh of vertices, containing an index and vertex buffer
    class mesh final {
    public:
        /// Constructs this mesh
        /// \param log_manager The log manager
        mesh(const std::shared_ptr<logging::ilog_manager>& log_manager) {
            assert((log_manager));

            if (!this->construct(log_manager)) {
                log_manager->log_message("Failed to construct mesh.",
                                         logging::log_levels::warning,
                                         "Graphics");
            }
        }

        /// Destroys this mesh
        ~mesh() {
            glDeleteVertexArrays(1, &this->_vao_id);
            glDeleteBuffers(1, &this->_vbo_id);
            glDeleteBuffers(1, &this->_index_buffer_id);
        }

        /// Renders this mesh
        void render() noexcept;

    private:
        /// The vertices
        std::vector<GLfloat> _vertices;

        /// The indices
        std::vector<GLushort> _indices;

        /// The vertex array object id
        GLuint _vao_id;

        /// The vertex buffer object id
        GLuint _vbo_id;

        /// The index buffer id
        GLuint _index_buffer_id;

        /// Constructs the buffers this mesh needs
        /// \returns `true` upon success, else `false`
        [[nodiscard]]
        bool construct(const std::shared_ptr<logging::ilog_manager>& log_manager) noexcept;
    };
}
