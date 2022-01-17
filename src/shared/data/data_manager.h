#pragma once

#include "settings.h"
#include "shared/apis/logging/ilog_manager.h"
#include "shared/apis/file/ifile_manager.h"

#include <optional>
#include <filesystem>
#include <memory>

namespace pbr::shared::data {
    /// Manages read and write access to the `data` directory. The format of the data is
    /// agnostic to the caller and is returned in a common format. File extensions are thus
    /// not required. JSON is used as the default data format.
    class data_manager {
    public:
        /// Constructs this data manager
        /// \param data_path The path to the `data` directory
        data_manager(std::filesystem::path data_path,
                     std::shared_ptr<apis::file::ifile_manager> file_manager,
                     std::shared_ptr<apis::logging::ilog_manager> log_manager)
            : _data_path(data_path),
                _file_manager(file_manager),
                _log_manager(log_manager) {
        }

        /// Destroys this data manager
        ~data_manager() = default;

        /// Reads a set of settings from the passed file
        /// \param relative_path The relative path to the settings file from the `data` directory
        /// \returns The read settings, else empty if an error occurred
        std::optional<settings> read_settings(const std::filesystem::path& relative_path) const noexcept;

    private:
        /// The path to the `data` directory
        std::filesystem::path _data_path;

        /// The file manager to use
        std::shared_ptr<apis::file::ifile_manager> _file_manager;

        /// The log manager to use
        std::shared_ptr<apis::logging::ilog_manager> _log_manager;

        /// Gets the full path of the passed relative path
        /// \param relative_path The relative path from the `data` directory
        /// \returns The full path of the passed relative path, else empty if a resolved path could
        /// not be found
        std::optional<std::filesystem::path> resolve_path(const std::filesystem::path& relative_path) const noexcept;
    };
}
