#include "datetime_manager.h"

#include <ctime>
#include <iomanip>
#include <sstream>

namespace pbr::shared::apis::datetime {
    std::string datetime_manager::get_date_as_utc_string() noexcept {
        return this->get_date_time_as_string("%Y-%m-%d");
    }

    std::string datetime_manager::get_time_as_utc_string() noexcept {
        return this->get_date_time_as_string("%H-%M-%S");
    }

    std::string datetime_manager::get_date_time_as_utc_string() noexcept {
        return this->get_date_time_as_string("%Y-%m-%d %H-%M-%S");
    }

    std::tm datetime_manager::get_now() noexcept {
        // TODO: Use `std::chrono::utc_clock` when clang supports it:
        // https://libcxx.llvm.org/Status/Cxx20.html
        //std::chrono::utc_clock c;

        auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

        tm tm;
#ifdef WIN32
        gmtime_s(&tm, &now);
#else
        gmtime_r(&now, &tm);
#endif

        return tm;
    }

    std::string datetime_manager::get_date_time_as_string(std::string_view format) noexcept {
        auto now = this->get_now();

        std::stringstream ss;
        ss << std::put_time(&now, std::string(format).c_str());

        auto result = ss.str();
        return result;
    }
}
