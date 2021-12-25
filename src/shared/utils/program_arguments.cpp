#include "program_arguments.h"

namespace pbr::shared::utils {
    bool program_arguments::setup_arguments(const std::vector<std::string>& arguments) noexcept {
        this->_executable_path = arguments[0];

        return true;
    }
}
