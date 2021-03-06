#pragma once

#include "shared/memory/basic_allocators.h"

#include <string>

namespace pbr::shared::apis::logging {
    /// The levels to log against
    enum class log_levels : char {
        /// For informational messages. This is the default log level
        info,

        /// For messages that require attention, but are not considered an error
        warning,

        /// For recoverable errors that require attention
        error,

        /// For unrecoverable errors that require a system halt.
        /// It is the responsibility of the caller to halt the system
        fatal,
    };

    /// Returns a string representation of the passed log level
    std::string to_string(log_levels level) noexcept;
}
