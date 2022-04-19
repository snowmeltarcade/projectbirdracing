#pragma once

#include <optional>
#include <string>

namespace pbr::shared::apis::graphics::opengl {
    // this can ONLY be called after GLEW has been initialized, as `glGetError` requires GLEW to set it up
    [[nodiscard]]
    std::optional<std::string> check_opengl_error_internal(const char* file_name, uint32_t line_number);

#ifdef DEBUG
#define CHECK_OPENGL_ERROR(log_manager) \
    if (auto error = pbr::shared::apis::graphics::opengl::check_opengl_error_internal(__FILE__, __LINE__); error) \
    { \
        log_manager->log_message("OpenGL Error: " + *error, logging::log_levels::error, "Graphics"); \
        return false; \
    }
#else
#define CHECK_OPENGL_ERROR
#endif
}
