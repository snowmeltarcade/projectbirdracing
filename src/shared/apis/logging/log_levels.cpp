#include "log_levels.h"

namespace pbr::shared::apis::logging {
    std::string to_string(log_levels level) noexcept {
        switch (level) {
            case log_levels::info: {
                return "info";
            }
            case log_levels::warning: {
                return "warning";
            }
            case log_levels::error: {
                return "error";
            }
            case log_levels::fatal: {
                return "fatal";
            }
        }

        return "unknown";
    }
}