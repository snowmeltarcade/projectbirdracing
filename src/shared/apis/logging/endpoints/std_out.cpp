#include "std_out.h"

#include <iostream>

namespace pbr::shared::apis::logging::endpoints {
    void std_out::log(std::string_view message, log_levels level) noexcept {
        if (level == log_levels::info) {
            std::cout << message << '\n';
        } else {
            std::cerr << message << '\n';
        }
    }
}
