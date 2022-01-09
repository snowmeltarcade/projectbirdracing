#include "file_manager.h"

namespace pbr::shared::apis::file {
    std::optional<std::vector<std::byte>> file_manager::read_file_data(const utils::uri& /*uri*/) const noexcept {
        return {};
    }
}
