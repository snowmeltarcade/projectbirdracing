#pragma once

#include "shared/memory/basic_allocators.h"
#include "shared/utils/uri.h"

#include <cstdint>
#include <vector>
#include <optional>
#include <future>

namespace pbr::shared::apis::file {
    /// Provides the interface for the file manager. This manager handles the loading file data
    /// from various endpoints, such as the file system, the network, a zip archive etc... A file
    /// path is specified by a URI. The protocol of the URI details from where the file should be
    /// retrieved. The protocols are not user configurable, but all supported protocols will be
    /// provided to all users of this manager.
    class ifile_manager {
    public:
        ifile_manager() = default;
        virtual ~ifile_manager() = default;

        /// Returns the bytes of the file pointed to by the passed URI
        /// \param uri The uri of the file to open
        /// \returns The bytes of the file pointed to by the passed URI. If this file was
        /// not found or an error occurred, an empty result is returned
        virtual std::optional<std::vector<std::byte>> read_file_bytes(const utils::uri& uri) const noexcept = 0;

        /// Returns the bytes of the file pointed to by the passed URI
        /// \param uri The uri of the file to open
        /// \returns The bytes of the file pointed to by the passed URI. If this file was
        /// not found or an error occurred, an empty result is returned
        virtual std::future<std::optional<std::vector<std::byte>>> read_file_bytes_async(
            const utils::uri& uri) const noexcept = 0;

        /// Returns the lines of text in the file pointed to by the passed URI
        /// \param uri The uri of the file to open
        /// \returns The the lines of text in the file pointed to by the passed URI. If this file was
        /// not found or an error occurred, an empty result is returned
        virtual std::optional<std::string> read_file_text(const utils::uri& uri) const noexcept = 0;

        /// Returns the lines of text in the file pointed to by the passed URI
        /// \param uri The uri of the file to open
        /// \returns The the lines of text in the file pointed to by the passed URI. If this file was
        /// not found or an error occurred, an empty result is returned
        virtual std::future<std::optional<std::string>> read_file_text_async(const utils::uri& uri) const noexcept = 0;
    };
}
