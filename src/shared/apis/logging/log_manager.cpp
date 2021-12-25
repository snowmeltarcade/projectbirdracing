#include "log_manager.h"

#include <sstream>

namespace pbr::shared::apis::logging {
    bool log_manager::add_endpoint(const std::shared_ptr<endpoint>& endpoint) noexcept {
        if (!endpoint) {
            return false;
        }

        std::scoped_lock<std::mutex> lock(this->_mutex);

        this->_endpoints.push_back(endpoint);

        return true;
    }

    const std::vector<std::shared_ptr<endpoint>>& log_manager::get_endpoints() const noexcept {
        std::scoped_lock<std::mutex> lock(this->_mutex);

        return this->_endpoints;
    }

    void log_manager::set_log_level(const log_levels level) noexcept {
        std::scoped_lock<std::mutex> lock(this->_mutex);

        this->_current_log_level = level;
    }

    log_levels log_manager::get_log_level() const noexcept {
        std::scoped_lock<std::mutex> lock(this->_mutex);

        return this->_current_log_level;
    }

    void log_manager::log_message(std::string_view message,
                                  const log_levels level,
                                  std::string_view prefix) noexcept {
        if (level < this->_current_log_level) {
            return;
        }

        // prefix: date time | level> message
        std::stringstream ss;

        if (!prefix.empty()) {
            ss << prefix << ": ";
        }

        ss << this->_datetime_manager->get_date_time_as_utc_string()
            << " | "
            << to_string(level)
            << "> "
            << message;

        auto formatted_message = ss.str();

        std::scoped_lock<std::mutex> lock(this->_mutex);

        for (const auto& e : this->_endpoints) {
            e->log(formatted_message, level);
        }
    }
}
