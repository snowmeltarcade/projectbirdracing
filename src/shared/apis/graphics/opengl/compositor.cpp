#include "compositor.h"
#include "mesh_shapes.h"
#include "render_targets/texture.h"
#include "opengl_errors.h"

namespace pbr::shared::apis::graphics::opengl {
    void compositor::render(const std::vector<std::shared_ptr<irender_target>>& submitted_targets) noexcept {
        if (submitted_targets.empty()) {
            //return;
        }

        // loop through submitted targets
            // get texture id
            // bind texture id
            //this->_mesh->render();

        this->_texture_target->bind();
        this->_texture_target->clear();
        this->_texture_shader->use();
        this->_texture_mesh->render();
        this->_texture_shader->clear_use();
        this->_texture_target->unbind();

        this->_destination->bind();
        this->_destination->clear();

        this->_shader_program->use();

        this->_mesh->render();

        this->_shader_program->clear_use();

        this->_destination->unbind();

        CHECK_OPENGL_ERROR_NO_RETURN(this->_log_manager);
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

        this->_texture_target = std::make_shared<render_targets::texture>(1024, 768, log_manager);
        this->_shader_program->bind_textures({ { "input_texture", this->_texture_target->texture_id() } });

        if (auto program = shader_program::create(log_manager, shader_manager,
                                                  { "color_vertex", "color_fragment" }); !program) {
            log_manager->log_message("Failed to create shader program.",
                                     logging::log_levels::error,
                                     "Graphics");
            return false;
        } else {
            this->_texture_shader = *program;
        }

        // the screen goes from -1..1, so we want 2.0f in width and height
        this->_mesh = create_rectangle(-1.0f, 1.0f, 0.0f, 2.0f, 2.0f, log_manager);
        assert((this->_mesh));

        this->_texture_mesh = create_rectangle(0.0f, 0.0f, 0.0f, 0.5f, 0.5f, log_manager);
        assert((this->_texture_mesh));

        CHECK_OPENGL_ERROR(log_manager);

        return true;
    }
}
