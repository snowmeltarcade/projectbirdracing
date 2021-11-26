#pragma once

#include "shared/memory/basic_allocators.h"
#include "shared/apis/logging/endpoint.h"

namespace pbr::shared::apis::logging::endpoints {
    /// Logs messages to `stdout` - AKA, the console
    class stdout : public endpoint {
    public:
        stdout() = default;
        ~stdout() override = default;

        /// Logs a message to this endpoint
        /// \param message The message to log. It is up to the caller to add any date, time or id etc... information
        // to this message
        void log(std::string_view message) noexcept override;
    };
}
