#include "mesh.h"

#include "opengl_errors.h"

namespace pbr::shared::apis::graphics::opengl {
    bool mesh::construct(const std::shared_ptr<logging::ilog_manager>& log_manager) noexcept {
        glGenVertexArrays(1, &this->_vao_id);

        glGenBuffers(1, &this->_vbo_id);
        glGenBuffers(1, &this->_index_buffer_id);

        glBindVertexArray(this->_vao_id);

        glBindBuffer(GL_ARRAY_BUFFER, this->_vbo_id);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), nullptr);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->_index_buffer_id);

        // this is for test
        this->_vertices = {
            -1.0f, -1.0f,
            1.0f, -1.0f,
            0.0f, 1.0f,
        };

        glBindBuffer(GL_ARRAY_BUFFER, this->_vbo_id);
        glBufferData(GL_ARRAY_BUFFER, this->_vertices.size() * sizeof(GLfloat), &this->_vertices[0], GL_STATIC_DRAW);

        this->_indices = {
            0, 1, 2,
        };

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->_index_buffer_id);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->_indices.size() * sizeof(GLushort), &this->_indices[0], GL_STATIC_DRAW);

        glBindVertexArray(0);

        CHECK_OPENGL_ERROR(log_manager);

        return true;
    }

    void mesh::render() noexcept {
        glBindVertexArray(this->_vao_id);

        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(this->_indices.size()), GL_UNSIGNED_SHORT, nullptr);

        glBindVertexArray(0);
    }
}
