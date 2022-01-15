#include <algorithm>

#include "strings.h"

namespace pbr::shared::utils {
    std::string get_characters_to_trim(char c) noexcept {
        const static std::string charactersToTrim = " \n\t\v\r";

        if (c == '\0') {
            return charactersToTrim;
        } else {
            // std::string has no constructor for `char`
            std::string s;
            s += c;
            return s;
        }
    }

    std::string trim(std::string_view s) noexcept {
        return rtrim(ltrim(s));
    }

	std::string trim(std::string_view s, char c) noexcept {
		return rtrim(ltrim(s, c), c);
	}

    std::string ltrim(std::string_view s) noexcept {
        auto value = ltrim(s, '\0');
        return value;
    }

	std::string ltrim(std::string_view s, char c) noexcept {
		auto i = s.find_first_not_of(get_characters_to_trim(c));

		// we must be all whitespace
		if (i == std::string::npos) {
		    return "";
        }

		s.remove_prefix(i);
		return std::string(s);
	}

	std::string rtrim(std::string_view s) noexcept {
        auto value = rtrim(s, '\0');
        return value;
	}

    std::string rtrim(std::string_view s, char c) noexcept {
        auto i = s.find_last_not_of(get_characters_to_trim(c)) + 1;

        if (i == std::string::npos || i >= s.length()) {
            return std::string(s);
        }

        s.remove_suffix(s.length() - i);

        return std::string(s);
    }

	std::string to_lower(std::string_view s) noexcept {
		std::string str(s);

		std::transform(str.begin(), str.end(), str.begin(),
			[](int c) { return static_cast<char>(std::tolower(c)); });

		return str;
	}

	std::string to_upper(std::string_view s) noexcept {
		std::string str(s);

		std::transform(str.begin(), str.end(), str.begin(),
			[](int c) { return static_cast<char>(std::toupper(c)); });

		return str;
	}

    std::vector<std::string> split(std::string_view divisor, const std::string& string_to_split) noexcept {
        std::vector<std::string> results;

        if (divisor.empty() || string_to_split.empty()) {
            return {};
        }

        size_t begin = 0u;

        while (true) {
            auto next = string_to_split.find(divisor, begin);
            if (next == std::string::npos) {
                break;
            }

            results.push_back(string_to_split.substr(begin, next-begin));
            begin = next + divisor.size();
        }

        if (begin <= string_to_split.size() - 1u) {
            results.push_back(string_to_split.substr(begin));
        }

        return results;
    }

    bool starts_with(std::string_view string_to_check, std::string_view starts_with) noexcept {
        if (string_to_check.empty() || starts_with.empty()) {
            return false;
        }

        auto res = string_to_check.substr(0, starts_with.size());
        return res == starts_with;
    }

    bool ends_with(std::string_view string_to_check, std::string_view ends_with) noexcept {
        if (string_to_check.empty() ||
            ends_with.empty() ||
            ends_with.size() > string_to_check.size()) {
            return false;
        }

        auto res = string_to_check.substr(string_to_check.size() - ends_with.size(), ends_with.size());
        return res == ends_with;
    }

    std::optional<int> to_int(std::string_view s) noexcept {
        try {
            return std::stoi(std::string(s), nullptr, 10);
        } catch (...) {
            return {};
        }
    }
}
