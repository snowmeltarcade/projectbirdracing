#include "mesh_shapes.h"

#include <vector>
#include <cassert>

namespace pbr::shared::apis::graphics::opengl {
    std::shared_ptr<mesh> create_rectangle(float left, float top, float z,
                                           float width, float height,
                                           const std::shared_ptr<logging::ilog_manager>& log_manager) noexcept {
        assert((log_manager));

        const glm::vec3 top_left(left, top, z);
        const glm::vec3 top_right(left + width, top, z);

        // OpenGL treats -y as down
        const glm::vec3 bottom_left(left, top - height, z);
        const glm::vec3 bottom_right(left + width, top - height, z);

        const std::vector<vertex> vertices {
            { top_left, { 1.0f, 0.8f, 0.0f, 1.0f, }, { 0.0f, 0.0f } },
            { top_right, { 0.0f, 0.3f, 0.5f, 1.0f, }, { 0.0f, 0.0f } },
            { bottom_left, { 0.5f, 0.1f, 1.0f, 1.0f, }, { 0.0f, 0.0f } },
            { bottom_right, { 0.5f, 0.1f, 1.0f, 1.0f, }, { 0.0f, 0.0f } },
        };

        static const std::vector<vertex_index_type> indices {
            0, 1, 3,
            0, 3, 2,
        };

        return std::make_shared<mesh>(vertices, indices, log_manager);
    }
}
