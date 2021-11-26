#pragma once

#include <cstdint>
#include <compare>

namespace pbr::shared::memory {
    class bytes;
    class megabytes;
    class gigabytes;

    /// Represents a number of kilobytes
    class kilobytes {
    public:
        /// The underlying type
        typedef double type;

        /// Construct this object
        /// \param value The value to set
        explicit kilobytes(type value) : _value(value) {}

        /// Construct this object from bytes
        /// \param value The bytes to construct from
        explicit kilobytes(bytes value);

        /// Construct this object from megabytes
        /// \param value The megabytes to construct from
        explicit kilobytes(megabytes value);

        /// Construct this object from gigabytes
        /// \param value The gigabytes to construct from
        explicit kilobytes(gigabytes value);

        kilobytes(const kilobytes&) = default;
        kilobytes(kilobytes&&) = default;
        ~kilobytes() = default;

        /// Returns the number of kilobytes
        type get_value() const noexcept {
            return this->_value;
        }

        /// Standard compare operators
        auto operator <=> (const kilobytes& other) const = default;
    private:
        /// The number of kilobytes
        type _value {0u};
    };
}
