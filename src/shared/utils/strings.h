#pragma once

#include "shared/memory/basic_allocators.h"

#include <string>
#include <array>
#include <cstdint>
#include <vector>

namespace pbr::shared::utils {
    /// Trims whitespace from both the beginning and end of the passed string. The
    /// whitespace characters are defined in `get_characters_to_trim()`
    /// \param s The string to trim
    /// \returns The trimmed string
	std::string trim(std::string_view s) noexcept;

    /// Trims whitespace from the beginning of the passed string. The
    /// whitespace characters are defined in `get_characters_to_trim()`
    /// \param s The string to trim
    /// \returns The trimmed string
	std::string ltrim(std::string_view s) noexcept;

    /// Trims whitespace from the end of the passed string. The
    /// whitespace characters are defined in `get_characters_to_trim()`
    /// \param s The string to trim
    /// \returns The trimmed string
	std::string rtrim(std::string_view s) noexcept;

    /// Trims the character `c` from both the beginning and end of the passed string
    /// \param s The string to trim
    /// \param c The character to trim from `s`
    /// \returns The trimmed string
    std::string trim(std::string_view s, char c) noexcept;

    /// Trims the character `c` from the beginning of the passed string
    /// \param s The string to trim
    /// \param c The character to trim from `s`
    /// \returns The trimmed string
    std::string ltrim(std::string_view s, char c) noexcept;

    /// Trims the character `c` from the end of the passed string
    /// \param s The string to trim
    /// \param c The character to trim from `s`
    /// \returns The trimmed string
    std::string rtrim(std::string_view s, char c) noexcept;

    /// Returns the passed string with all characters in lower case
    /// \param s The string to process
    /// \returns The passed string with all characters in lower case
	std::string to_lower(std::string_view s) noexcept;

    /// Returns the passed string with all characters in upper case
    /// \param s The string to process
    /// \returns The passed string with all characters in upper case
	std::string to_upper(std::string_view s) noexcept;

    /// Converts the passed string to an `std::array<>`
    /// \param s The string to convert
    /// \returns An array converted to from the passed string
	template <size_t N>
	std::array<uint8_t, N> to_array(std::string_view s) noexcept {
        std::array<uint8_t, N> a {};

        auto size = s.size();

        for (auto i = 0u; i < N; ++i) {
            a[i] = (i < size) ? s[i] : '\0';
        }

        return a;
    }

    /// Converts the passed array to a `std::string`
    /// \param a The array to convert
    /// \returns A string converted to from the passed array
    template <size_t N>
    std::string from_array(const std::array<uint8_t, N>& a) noexcept {
        std::string s;

        for (auto i = 0u; i < N; ++i) {
            if (a[i] == '\0') {
                break;
            }

            s += a[i];
        }

        return s;
    }

    /// Returns the default characters that are trimmed
    /// \returns The default characters that are trimmed
	[[nodiscard]]
    std::string get_characters_to_trim() noexcept;

    /// Splits the passed string by the passed divisor. If the divisor is not found in the passed string,
    /// the return value will contain the passed string
    /// \param divisor The divisor to split the string by
    /// \param string_to_split The string to split
    /// \returns A vector containing the parts of the split string
	std::vector<std::string> split(std::string_view divisor, const std::string& string_to_split) noexcept;

    /// Returns true if the passed string starts with `starts_with`
    /// \param string_to_check The string to check
    /// \param starts_with The value to check for
    /// \returns `true` if the passed string starts with `starts_with`, else `false`
    bool starts_with(std::string_view string_to_check, std::string_view starts_with) noexcept;

    /// Returns true if the passed string ends with `ends_with`
    /// \param string_to_check The string to check
    /// \param ends_with The value to check for
    /// \returns `true` if the passed string ends with `ends_with`, else `false`
    bool ends_with(std::string_view string_to_check, std::string_view ends_with) noexcept;
}
