#pragma once

#include <unordered_map>
#include <string>

namespace pbr::shared::diagnostics {
    /// A set of counters. A counter can be incremented, decremented and have various
    /// average based operations performed against it. It can also be reset. This may
    /// be useful for frame counters or number of render calls per frame etc... Counters
    /// are referenced by a string based key.
    class counter_set {
    public:
        /// Increments a counter
        /// \param key The key of the counter
        /// \param amount The amount to increment by
        void increment(const std::string& key, int amount) noexcept;

        /// Decrements a counter
        /// \param key The key of the counter
        /// \param amount The amount to decrement by
        void decrement(const std::string& key, int amount) noexcept;

    private:
        /// Stores the counters
        std::unordered_map<std::string, int> _counters;
    };
}
