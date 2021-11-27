#pragma once

#include "idatetime_manager.h"

namespace pbr::shared::apis::datetime {
    /// Handles the date and time of the current running machine, not in game time
    class datetime_manager : public idatetime_manager {
    public:
        datetime_manager() = default;
        ~datetime_manager() override = default;

        /// Returns the current date in UTC as a string
        /// \returns The current date in UTC as a string
        [[nodiscard]]
        std::string get_date_as_utc_string() noexcept override;

        /// Returns the current time in UTC as a string
        /// \returns The current time in UTC as a string
        [[nodiscard]]
        std::string get_time_as_utc_string() noexcept override;

        /// Returns the current date and time in UTC as a string
        /// \returns The current date and time in UTC as a string
        [[nodiscard]]
        std::string get_date_time_as_utc_string() noexcept override;

    private:
        /// Returns the duration since epoch
        /// \returns The duration since epoch
        [[nodiscard]]
        std::tm get_now() noexcept;

        /// Returns the current time in UTC formatted by `format`
        /// \param format The format of the date time
        /// \returns The formatted date time
        [[nodiscard]]
        std::string get_date_time_as_string(std::string_view format) noexcept;
    };
}
