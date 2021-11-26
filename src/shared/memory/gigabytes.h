#pragma once

#include <cstdint>
#include <compare>

namespace pbr::shared::memory {
    class bytes;
    class kilobytes;
    class megabytes;

    /// Represents a number of gigabytes
    class gigabytes {
    public:
        /// The underlying type
        typedef double type;

        /// Construct this object
        /// \param value The value to set
        explicit gigabytes(type value) : _value(value) {}

        /// Construct this object from bytes
        /// \param value The bytes to construct from
        explicit gigabytes(bytes value);

        /// Construct this object from kilobytes
        /// \param value The kilobytes to construct from
        explicit gigabytes(kilobytes value);

        /// Construct this object from megabytes
        /// \param value The megabytes to construct from
        explicit gigabytes(megabytes value);

        gigabytes(const gigabytes&) = default;
        gigabytes(gigabytes&&) = default;
        ~gigabytes() = default;

        /// Returns the number of gigabytes
        type get_value() const noexcept {
            return this->_value;
        }

        /// Standard compare operators
        auto operator <=> (const gigabytes& other) const = default;
    private:
        /// The number of gigabytes
        type _value {0u};
    };
}
