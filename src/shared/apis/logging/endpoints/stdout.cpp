#include "stdout.h"

#include <iostream>

namespace pbr::shared::apis::logging::endpoints {
    void stdout::log(std::string_view message) noexcept {
        std::cout << message << '\n';
    }
}
