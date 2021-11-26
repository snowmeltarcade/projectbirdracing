#include "log_manager.h"
#include "shared/apis/datetime/datetime_manager.h"

#include <mutex>
#include <sstream>

namespace pbr::shared::apis::logging {
    /// Helps ensure logging is thread safe
    static std::mutex g_mutex;

    /// The endpoints to log against
    static std::vector<std::shared_ptr<endpoint>> g_endpoints;

    /// The current log level
    static log_levels g_current_log_level { log_levels::info };

    /// The datetime manager to use
    static datetime::datetime_manager g_datetime_manager;

    bool add_endpoint(const std::shared_ptr<endpoint>& endpoint) noexcept {
        if (!endpoint) {
            return false;
        }

        std::scoped_lock<std::mutex> lock(g_mutex);

        g_endpoints.push_back(endpoint);

        return true;
    }

    const std::vector<std::shared_ptr<endpoint>>& get_endpoints() noexcept {
        std::scoped_lock<std::mutex> lock(g_mutex);

        return g_endpoints;
    }

    void set_log_level(const log_levels level) noexcept {
        std::scoped_lock<std::mutex> lock(g_mutex);

        g_current_log_level = level;
    }

    log_levels get_log_level() noexcept {
        std::scoped_lock<std::mutex> lock(g_mutex);

        return g_current_log_level;
    }

    void log_message(std::string_view message, const log_levels level) noexcept {
        if (level < g_current_log_level) {
            return;
        }

        // date time | level> message
        std::stringstream ss;
        ss << g_datetime_manager.get_date_time_as_utc_string()
            << " | "
            << to_string(level)
            << "> "
            << message;

        auto formatted_message = ss.str();

        std::scoped_lock<std::mutex> lock(g_mutex);

        for (const auto& e : g_endpoints) {
            e->log(formatted_message);
        }
    }
}
