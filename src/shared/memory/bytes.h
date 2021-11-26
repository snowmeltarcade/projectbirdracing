#pragma once

#include <cstdint>
#include <compare>

namespace pbr::shared::memory {
    class kilobytes;
    class megabytes;
    class gigabytes;

    /// Represents a number of bytes
    class bytes {
    public:
        /// The underlying type
        typedef uint64_t type;

        /// Construct this object
        /// \param value The value to set
        explicit bytes(type value) : _value(value) {}

        /// Construct this object from kilobytes
        /// \param value The kilobytes to construct from
        explicit bytes(kilobytes value);

        /// Construct this object from megabytes
        /// \param value The megabytes to construct from
        explicit bytes(megabytes value);

        /// Construct this object from gigabytes
        /// \param value The gigabytes to construct from
        explicit bytes(gigabytes value);

        bytes(const bytes&) = default;
        bytes(bytes&&) = default;
        ~bytes() = default;

        /// Returns the number of bytes
        type get_value() const noexcept {
            return this->_value;
        }

        /// Standard compare operators
        auto operator <=> (const bytes& other) const = default;

        /// Addition
        bytes operator + (const bytes& other) const {
            return bytes(this->get_value() + other.get_value());
        }

        /// Subtraction
        bytes operator - (const bytes& other) const {
            return bytes(this->get_value() - other.get_value());
        }
    private:
        /// The number of bytes
        type _value {0u};
    };
}
