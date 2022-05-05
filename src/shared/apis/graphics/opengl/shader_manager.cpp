#include "shader_manager.h"

namespace pbr::shared::apis::graphics::opengl {
    std::shared_ptr<shader> shader_manager::load(const std::filesystem::path& path) noexcept {
        auto created_shader = std::make_shared<shader>(path, this->_data_manager, this->_log_manager);
        return created_shader;
    }
}
