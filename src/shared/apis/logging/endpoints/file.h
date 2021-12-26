#pragma once

#include "shared/memory/basic_allocators.h"
#include "shared/apis/logging/endpoint.h"

#include <mutex>
#include <unordered_map>
#include <filesystem>

namespace pbr::shared::apis::logging::endpoints {
    /// Logs messages to a file
    class file : public endpoint {
    public:
        /// Constructs this endpoint
        /// \param path The path of the file
        /// \param overwrite `true` to overwrite the file (delete the file in this constructor), `false` to append
        file(const std::filesystem::path& path, bool overwrite = false)
            : _path(path)
        {
            if (overwrite) {
                std::filesystem::remove(path);
            }
        }

        /// Destructs this endpoint
        ~file() override = default;

        /// Logs a message to this endpoint
        /// \param message The message to log. It is up to the caller to add any date, time or id etc... information
        // to this message
        /// \param level The log level
        void log(std::string_view message, log_levels level) noexcept override;

    private:
        /// Protects concurrent access to specific files
        static std::unordered_map<std::string, std::mutex> _mutexes;

        /// The path of the file
        std::filesystem::path _path;
    };
}
