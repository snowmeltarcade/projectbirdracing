#pragma once

#include "shared/memory/basic_allocators.h"
#include "log_levels.h"
#include "endpoint.h"

#include <memory>
#include <vector>

namespace pbr::shared::apis::logging {
    /// Adds a logging endpoint
    /// \param endpoint A pointer to the endpoint to add. If nullptr, `false` will be returned
    /// \returns `true` on success, else `false`
    [[nodiscard]]
    bool add_endpoint(const std::shared_ptr<endpoint>& endpoint) noexcept;

    /// Returns the added endpoints
    /// \returns The added endpoints
    [[nodiscard]]
    const std::vector<std::shared_ptr<endpoint>>& get_endpoints() noexcept;

    /// Sets the log level to log against. Any messages that are logged with a level below this will
    /// be ignored
    /// \param level The log level to set
    void set_log_level(const log_levels level) noexcept;

    /// Returns the current log level
    /// \returns The current log level
    [[nodiscard]]
    log_levels get_log_level() noexcept;

    /// Logs a message to all added endpoints. If the set log level is less than the passed level,
    /// the message will not be logged. The level, current date and time in UTC will be prefixed to the message
    /// \param message The message to log
    /// \param level The logging level to log against
    void log_message(std::string_view message, const log_levels level) noexcept;
}
