#pragma once

#include <string>
#include <optional>

namespace pbr::shared::utils {
    /// Represents a uri.
    /// See: https://en.wikipedia.org/wiki/Uniform_Resource_Identifier#Design
    /// and https://datatracker.ietf.org/doc/html/rfc3986
    struct uri {
        /// The scheme
        std::string scheme;

        /// The path
        std::string path;

        /// The default equality operator
        /// \param other What to compare against
        /// \returns `true` if equal, else `false`
        bool operator ==(const uri& other) const = default;
    };

    /// Builds a URI from the passed string
    /// \param s The string to build the URI from
    /// \returns The URI, or empty if the URI could not be constructed
    [[nodiscard]]
    std::optional<uri> build_uri(const std::string& s) noexcept;
}
