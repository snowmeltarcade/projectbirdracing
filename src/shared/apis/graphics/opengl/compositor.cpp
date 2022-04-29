#include "compositor.h"

namespace pbr::shared::apis::graphics::opengl {
    void compositor::render(const std::vector<std::shared_ptr<irender_target>>& submitted_targets) noexcept {
        if (submitted_targets.empty()) {
            //return;
        }

        // loop through submitted targets
            // get texture id
            // bind texture id
            //this->_mesh->render();

        this->_destination->submit();
        this->_destination->bind();

        this->_shader_program->use();

        this->_mesh->render();

        this->_shader_program->clear_use();

        this->_destination->unbind();
    }

    bool compositor::load_resources(const std::shared_ptr<shader_manager>& shader_manager,
                                    const std::shared_ptr<logging::ilog_manager>& log_manager) noexcept {
        assert((shader_manager));
        assert((log_manager));

        if (auto program = shader_program::create(log_manager, shader_manager,
                                                  { "compositor_vertex", "compositor_fragment" }); !program) {
            log_manager->log_message("Failed to create shader program.",
                                     logging::log_levels::error,
                                     "Graphics");
            return false;
        } else {
            this->_shader_program = *program;
        }

        this->_mesh = std::make_shared<mesh>(log_manager);

        return true;
    }
}
