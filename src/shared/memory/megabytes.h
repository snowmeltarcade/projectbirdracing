#pragma once

#include <cstdint>
#include <compare>

namespace pbr::shared::memory {
    class bytes;
    class kilobytes;
    class gigabytes;

    /// Represents a number of megabytes
    class megabytes {
    public:
        /// The underlying type
        typedef double type;

        /// Construct this object
        /// \param value The value to set
        explicit megabytes(type value) : _value(value) {}

        /// Construct this object from bytes
        /// \param value The bytes to construct from
        explicit megabytes(bytes value);

        /// Construct this object from kilobytes
        /// \param value The kilobytes to construct from
        explicit megabytes(kilobytes value);

        /// Construct this object from gigabytes
        /// \param value The gigabytes to construct from
        explicit megabytes(gigabytes value);

        megabytes(const megabytes&) = default;
        megabytes(megabytes&&) = default;
        ~megabytes() = default;

        /// Returns the number of megabytes
        type get_value() const noexcept {
            return this->_value;
        }

        /// Standard compare operators
        auto operator <=> (const megabytes& other) const = default;
    private:
        /// The number of megabytes
        type _value {0u};
    };
}
