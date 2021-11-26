#include "log_manager.h"

#include <mutex>

namespace pbr::shared::apis::logging {
    /// Helps ensure logging is thread safe
    static std::mutex g_mutex;

    /// The endpoints to log against
    static std::vector<std::shared_ptr<endpoint>> g_endpoints;

    /// The current log level
    static log_levels g_current_log_level { log_levels::info };

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
        g_current_log_level = level;
    }

    log_levels get_log_level() noexcept {
        return g_current_log_level;
    }
}
