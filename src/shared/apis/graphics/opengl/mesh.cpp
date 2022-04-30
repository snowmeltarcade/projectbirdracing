#include "mesh.h"
#include "opengl_errors.h"

namespace pbr::shared::apis::graphics::opengl {
    /// Sets the attribute pointer information
    /// \param attribute_index The index of the attribute
    /// \param element_count The number of elements
    /// \param offset The offset of this attribute
    /// \returns The offset to use for the next attribute (discard-able)
    int set_vertex_attribute(int attribute_index, int element_count, int offset) {
        glEnableVertexAttribArray(attribute_index);

        glVertexAttribPointer(attribute_index,
                              element_count,
                              GL_FLOAT,
                              GL_FALSE,
                              sizeof(vertex),
                              reinterpret_cast<void*>(offset));

        auto total_element_size = element_count * sizeof(GLfloat);

        return offset + total_element_size;
    }

    /// Sets up the vertex attributes based on the layout of `vertex`
    void setup_vertex_attributes() {
        auto offset {0u};
        auto attribute_index {0};

        // position
        offset = set_vertex_attribute(attribute_index++, 3, offset);

        // color
        offset = set_vertex_attribute(attribute_index++, 4, offset);

        // texture coords
        set_vertex_attribute(attribute_index, 2, offset);
    }

    bool mesh::construct(const std::shared_ptr<logging::ilog_manager>& log_manager) noexcept {
        glGenVertexArrays(1, &this->_vao_id);

        glGenBuffers(1, &this->_vbo_id);
        glGenBuffers(1, &this->_index_buffer_id);

        glBindVertexArray(this->_vao_id);

        glBindBuffer(GL_ARRAY_BUFFER, this->_vbo_id);

        setup_vertex_attributes();

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->_index_buffer_id);

        // this is for test
        this->_vertices = {
            { { -1.0f, -1.0f, 0.0f, }, { 1.0f, 0.8f, 0.0f, 1.0f, }, { 0.0f, 0.0f } },
            { { 1.0f, -1.0f, 0.0f, }, { 0.0f, 0.3f, 0.5f, 1.0f, }, { 0.0f, 0.0f } },
            { { 0.0f, 1.0f, 0.0f, }, { 0.5f, 0.1f, 1.0f, 1.0f, }, { 0.0f, 0.0f } },
        };

        glBindBuffer(GL_ARRAY_BUFFER, this->_vbo_id);
        glBufferData(GL_ARRAY_BUFFER, this->_vertices.size() * sizeof(vertex), &this->_vertices[0], GL_STATIC_DRAW);

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
