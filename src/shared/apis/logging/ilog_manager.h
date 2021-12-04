#pragma once

#include "log_levels.h"
#include "endpoint.h"

#include <memory>
#include <string>
#include <vector>

namespace pbr::shared::apis::logging {
    /// Provides an interface to the log manager. This manager allows you to add logging endpoints
    /// which will be output a log message to that endpoint. These endpoints could be `stdout`, or
    /// a file for instance.
    class ilog_manager {
    public:
        ilog_manager() = default;
        virtual ~ilog_manager() = default;

        /// Adds a logging endpoint
        /// \param endpoint A pointer to the endpoint to add. If nullptr, `false` will be returned
        /// \returns `true` on success, else `false`
        [[nodiscard]]
        virtual bool add_endpoint(const std::shared_ptr<endpoint>& endpoint) noexcept = 0;

        /// Returns the added endpoints
        /// \returns The added endpoints
        [[nodiscard]]
        virtual const std::vector<std::shared_ptr<endpoint>>& get_endpoints() const noexcept = 0;

        /// Sets the log level to log against. Any messages that are logged with a level below this will
        /// be ignored
        /// \param level The log level to set
        virtual void set_log_level(const log_levels level) noexcept = 0;

        /// Returns the current log level
        /// \returns The current log level
        [[nodiscard]]
        virtual log_levels get_log_level() const noexcept = 0;

        /// Logs a message to all added endpoints. If the set log level is less than the passed level,
        /// the message will not be logged. The level, current date and time in UTC will be prefixed to the message
        /// \param message The message to log
        /// \param level The logging level to log against
        virtual void log_message(std::string_view message, const log_levels level) noexcept = 0;
    };
}
