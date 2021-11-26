#pragma once

#include "shared/memory/basic_allocators.h"

#include <string>
#include <chrono>

namespace pbr::shared::apis::datetime {
    /// Provides the API interface for the datetime manager. This manager handles the date and time
    /// of the current running machine, not in game time
    class idatetime_manager {
    public:
        /// Returns the current date in UTC as a string
        /// \returns The current date in UTC as a string
        virtual std::string get_date_as_utc_string() noexcept = 0;

        /// Returns the current time in UTC as a string
        /// \returns The current time in UTC as a string
        virtual std::string get_time_as_utc_string() noexcept = 0;

        /// Returns the current date and time in UTC as a string
        /// \returns The current date and time in UTC as a string
        virtual std::string get_date_time_as_utc_string() noexcept = 0;
    };
}
