#pragma once

#include "log_levels.h"

#include <string>

namespace pbr::shared::apis::logging {
    /// An interface to a logging endpoint. The implementations are not required to be thread safe as the
    /// logging system itself will handle that
    class endpoint {
    public:
        endpoint() = default;
        virtual ~endpoint() = default;

        /// Logs a message to this endpoint
        /// \param message The message to log. It is up to the caller to add any date, time or id etc... information
        // to this message
        /// \param level The log level
        virtual void log(std::string_view message, log_levels level) noexcept = 0;
    };
}
