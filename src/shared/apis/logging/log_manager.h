#pragma once

#include "shared/memory/basic_allocators.h"
#include "ilog_manager.h"
#include "log_levels.h"
#include "shared/apis/datetime/idatetime_manager.h"

#include <mutex>
#include <cassert>

namespace pbr::shared::apis::logging {
    /// The log manager allows you to add logging endpoints which will be output a log message to that endpoint.
    /// These endpoints could be `stdout`, or a file for instance.
    class log_manager : public ilog_manager {
    public:
        /// Constructs the log manager
        /// \param datetime_manager The datetime manager to use
        log_manager(std::shared_ptr<datetime::idatetime_manager> datetime_manager)
            : _datetime_manager(std::move(datetime_manager)) {
            assert((this->_datetime_manager));
        }
        ~log_manager() override = default;

        /// Adds a logging endpoint
        /// \param endpoint A pointer to the endpoint to add. If nullptr, `false` will be returned
        /// \returns `true` on success, else `false`
        [[nodiscard]]
        bool add_endpoint(const std::shared_ptr<endpoint>& endpoint) noexcept override;

        /// Returns the added endpoints
        /// \returns The added endpoints
        [[nodiscard]]
        const std::vector<std::shared_ptr<endpoint>>& get_endpoints() const noexcept override;

        /// Sets the log level to log against. Any messages that are logged with a level below this will
        /// be ignored
        /// \param level The log level to set
        void set_log_level(const log_levels level) noexcept override;

        /// Returns the current log level
        /// \returns The current log level
        [[nodiscard]]
        log_levels get_log_level() const noexcept override;

        /// Logs a message to all added endpoints. If the set log level is less than the passed level,
        /// the message will not be logged. The level, current date and time in UTC will be prefixed to the message
        /// \param message The message to log
        /// \param level The logging level to log against
        /// \param key A prefix to the message
        void log_message(std::string_view message,
                         const log_levels level,
                         std::string_view prefix = "") noexcept override;

    private:
        /// Helps ensure logging is thread safe
        mutable std::mutex _mutex;

        /// The endpoints to log against
        std::vector<std::shared_ptr<endpoint>> _endpoints;

        /// The current log level
        log_levels _current_log_level { log_levels::info };

        /// The datetime manager to use
        std::shared_ptr<datetime::idatetime_manager> _datetime_manager;
    };
}
