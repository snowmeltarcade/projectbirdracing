#pragma once

#include "shared/apis/graphics/shader_types.h"

#include <string>

namespace pbr::shared::data {
    /// The code is a shader along with the type of shader this code is for
    struct shader_code final {
        /// The code
        std::string code;

        /// The shader type
        apis::graphics::shader_types type {apis::graphics::shader_types::vertex};
    };
}
