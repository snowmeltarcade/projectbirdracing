#include "file.h"

#include <fstream>

namespace pbr::shared::apis::logging::endpoints {
    std::unordered_map<std::string, std::mutex> file::_mutexes;

    void file::log(std::string_view message, log_levels) noexcept {
        std::scoped_lock<std::mutex> lock(file::_mutexes[std::string(this->_path)]);

        std::ofstream fs(this->_path, std::ios_base::app);
        fs << message << '\n';
        fs.close();
    }
}
