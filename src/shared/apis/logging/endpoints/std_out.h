#pragma once

#include "shared/memory/basic_allocators.h"
#include "shared/apis/logging/endpoint.h"

namespace pbr::shared::apis::logging::endpoints {
    /// Logs messages to `stdout` - AKA, the console
    class std_out : public endpoint {
    public:
        std_out() = default;
        ~std_out() override = default;

        /// Logs a message to this endpoint
        /// \param message The message to log. It is up to the caller to add any date, time or id etc... information
        // to this message
        /// \param level The log level
        void log(std::string_view message, log_levels level) noexcept override;
    };
}
