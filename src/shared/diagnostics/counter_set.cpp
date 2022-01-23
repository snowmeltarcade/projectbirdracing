#include "counter_set.h"

#include <numeric>

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
                                              std::chrono::milliseconds duration,
                                              int& out_result_at_duration) noexcept {
        auto now = std::chrono::system_clock::now();
        auto last_update = this->_counter_update_times[key];

        auto counter = this->_counters[key];

        if (now - last_update >= duration) {
            this->_counter_update_times[key] = now;
            this->_counters[key] = 0;
            out_result_at_duration = counter;
            return 0;
        }

        return counter;
    }

    float counter_set::get_average_for_duration(const std::string& key,
                                                std::chrono::milliseconds duration,
                                                float& out_average_at_duration) noexcept {
        auto now = std::chrono::system_clock::now();
        auto last_update = this->_counter_update_times[key];

        auto& v = this->_counter_lists[key];
        auto sum = std::reduce(v.begin(), v.end());

        auto result = sum / static_cast<float>(v.size());

        if (now - last_update >= duration) {
            this->_counter_update_times[key] = now;
            this->_counter_lists[key].clear();
            out_average_at_duration = result;
        }

        return result;
    }
}
