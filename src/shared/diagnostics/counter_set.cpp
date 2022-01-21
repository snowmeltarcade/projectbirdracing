#include "counter_set.h"

namespace pbr::shared::diagnostics {
    void counter_set::increment_counter(const std::string& key, int amount) noexcept {
        this->_counters[key] += amount;
    }

    void counter_set::decrement_counter(const std::string& key, int amount) noexcept {
        this->_counters[key] -= amount;
    }

    void counter_set::add_value_to_list(const std::string& key, int value) noexcept {
        this->_counter_lists[key].push_back(value);
    }

    int counter_set::get_counter_for_duration(const std::string& key,
                                              std::chrono::milliseconds duration) noexcept {
        auto now = std::chrono::system_clock::now();
        auto last_update = this->_counter_update_times[key];

        if (now - last_update > duration) {
            this->_counter_update_times[key] = now;
            this->_counters[key] = 0;
        }

        auto counter = this->_counters[key];
        return counter;
    }

    int counter_set::get_average_for_duration(const std::string&,
                                              std::chrono::milliseconds) noexcept {
        return 0;
    }
}
