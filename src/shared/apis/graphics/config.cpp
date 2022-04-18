#include "config.h"

namespace pbr::shared::apis::graphics {
    bool config::load(const std::shared_ptr<data::data_manager>&,
                      const std::shared_ptr<logging::ilog_manager>&) noexcept {
        return true;
    }
}
