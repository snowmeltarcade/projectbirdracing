#include "compositor.h"

namespace pbr::shared::apis::graphics::opengl {
    void compositor::render(const std::vector<std::shared_ptr<irender_target>>& submitted_targets) noexcept {
        if (submitted_targets.empty()) {
            return;
        }

        // bind default target

        // bind shader and vbo

        // loop through submitted targets
            // get texture id
            // bind texture id
            // render

        // submit default target
    }
}
