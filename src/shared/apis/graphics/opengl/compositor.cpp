#include "compositor.h"

namespace pbr::shared::apis::graphics::opengl {
    void compositor::render(const std::vector<std::shared_ptr<irender_target>>& submitted_targets) noexcept {
        if (submitted_targets.empty()) {
            //return;
        }

        this->_destination->bind();

        this->_shader_program->use();

        // bind shader and vbo

        // loop through submitted targets
            // get texture id
            // bind texture id
            // render

        this->_destination->submit();

        this->_shader_program->clear_use();

        this->_destination->unbind();
    }

    bool compositor::load_resources(const std::shared_ptr<shader_manager>& shader_manager,
                                    const std::shared_ptr<logging::ilog_manager>& log_manager) noexcept {
        assert((shader_manager));
        assert((log_manager));

        this->_shader_program = std::make_shared<shader_program>(log_manager);

        auto vertex_shader = shader_manager->get("compositor_vertex");
        if (!vertex_shader) {
            log_manager->log_message("Failed to load vertex shader: `compositor_vertex`.",
                                     logging::log_levels::error,
                                     "Graphics");
            return false;
        }

        if (!this->_shader_program->attach_shader(vertex_shader)) {
            log_manager->log_message("Failed to attach vertex shader: `compositor_vertex`.",
                                     logging::log_levels::error,
                                     "Graphics");
            return false;
        }

        auto fragment_shader = shader_manager->get("compositor_fragment");
        if (!fragment_shader) {
            log_manager->log_message("Failed to load fragment shader: `compositor_fragment`.",
                                     logging::log_levels::error,
                                     "Graphics");
            return false;
        }

        if (!this->_shader_program->attach_shader(fragment_shader)) {
            log_manager->log_message("Failed to attach vertex shader: `compositor_fragment`.",
                                     logging::log_levels::error,
                                     "Graphics");
            return false;
        }

        if (!this->_shader_program->link()) {
            log_manager->log_message("Failed to link compositors shader program.",
                                     logging::log_levels::error,
                                     "Graphics");
            return false;
        }

        return true;
    }
}
