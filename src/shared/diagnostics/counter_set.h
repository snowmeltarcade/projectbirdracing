#pragma once

#include <unordered_map>
#include <string>
#include <chrono>
#include <vector>

namespace pbr::shared::diagnostics {
    /// A set of counters. A counter can be incremented, decremented and have various
    /// average based operations performed against it. It can also be reset. This may
    /// be useful for frame counters or number of render calls per frame etc... Counters
    /// are referenced by a string based key. If a requested key does not exist, a key is
    /// created with the default type of the counter
    class counter_set {
    public:
        /// Increments a counter
        /// \param key The key of the counter
        /// \param amount The amount to increment by
        void increment_counter(const std::string& key, int amount) noexcept;

        /// Decrements a counter
        /// \param key The key of the counter
        /// \param amount The amount to decrement by
        void decrement_counter(const std::string& key, int amount) noexcept;

        /// Adds a value to a counter list
        /// \param key The key of the counter
        /// \param value The value to add
        void add_value_to_list(const std::string& key, int value) noexcept;

        /// Returns the counter for the passed key
        /// \param key The key of the counter
        /// \returns The counter for the passed key
        int get_counter(const std::string& key) noexcept {
            return this->_counters[key];
        }

        /// Returns the values for the counter list for the passed key
        /// \param key The key of the counter list
        /// \returns The values for the counter list for the passed key
        const std::vector<int>& get_values_for_counter_list(const std::string& key) noexcept {
            return this->_counter_lists[key];
        }

        /// Returns the count indicated by the passed key. If the time since this function was last
        /// called is greater than or equal to duration, the count is reset. The first time the duration
        /// is hit, the result will not be accurate, as a new key's last update time will default to
        /// the default value of `time_point`, call this function at last once before actual use to
        /// ensure the last update time is set correctly
        /// \param key The key of the counter
        /// \param duration The duration to reset the count after in milliseconds
        /// \param out_result_at_duration The count at the point when `duration` has been reached, else
        /// this value is not set
        /// \returns The count
        // no `[[nodiscard]]`
        int get_counter_for_duration(const std::string& key,
                                     std::chrono::milliseconds duration,
                                     int& out_result_at_duration) noexcept;

        /// Returns the average of a counter list indicated by the passed key. If the time since this
        /// function was last called is greater than or equal to duration, the counter list is reset
        /// \param key The key of the counter
        /// \param duration The duration to reset the count after in milliseconds
        /// \param out_average_at_duration The average at the point when `duration` has been reached, else
        /// this value is not set
        /// \returns The average of the counter list
        // no `[[nodiscard]]`
        float get_average_for_duration(const std::string& key,
                                       std::chrono::milliseconds duration,
                                       float& out_average_at_duration) noexcept;

    private:
        /// Stores the counters
        std::unordered_map<std::string, int> _counters;

        /// When a counter was last updated
        std::unordered_map<std::string, std::chrono::system_clock::time_point> _counter_update_times;

        /// Stores the counter lists
        std::unordered_map<std::string, std::vector<int>> _counter_lists;
    };
}
