#pragma once

#include <string>

namespace pbr::shared::utils {
    /// Converts an int to a `std::string`
    /// \param value The value to convert
    /// \returns The string converted value
    std::string to_string(int value) noexcept;

    /// Converts a bool to a `std::string`
    /// \param value The value to convert
    /// \returns The string converted value
    std::string to_string(bool value) noexcept;

    /// Ensures that this type can be converted to a string
    /// \tparam T The type to convert
    template <typename T>
    concept stringable = requires (T t) {
        to_string(t);
    } || std::is_convertible_v<T, std::string>;
}
