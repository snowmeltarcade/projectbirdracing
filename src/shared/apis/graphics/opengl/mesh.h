#pragma once

#include "shared/apis/logging/ilog_manager.h"
#include "opengl_dependencies.h"
#include "vertex.h"

#include <memory>
#include <vector>
#include <cassert>

namespace pbr::shared::apis::graphics::opengl {
    /// A vertex index
    typedef GLushort vertex_index_type;

    /// A mesh of vertices, containing an index and vertex buffer. The vertex and index data
    /// is not saved.
    class mesh final {
    public:
        /// Constructs this mesh
        /// \param vertices The vertex data
        /// \param indices The index data
        /// \param log_manager The log manager
        mesh(const std::vector<vertex>& vertices,
             const std::vector<vertex_index_type>& indices,
             const std::shared_ptr<logging::ilog_manager>& log_manager) {
            assert((log_manager));

            if (!this->construct(vertices, indices, log_manager)) {
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
        /// The vertex array object id
        GLuint _vao_id;

        /// The vertex buffer object id
        GLuint _vbo_id;

        /// The index buffer id
        GLuint _index_buffer_id;

        /// The number of indices
        size_t _index_count;

        /// Constructs the buffers this mesh needs
        /// \param vertices The vertex data
        /// \param indices The index data
        /// \param log_manager The log manager
        /// \returns `true` upon success, else `false`
        [[nodiscard]]
        bool construct(const std::vector<vertex>& vertices,
                       const std::vector<vertex_index_type>& indices,
                       const std::shared_ptr<logging::ilog_manager>& log_manager) noexcept;
    };
}
