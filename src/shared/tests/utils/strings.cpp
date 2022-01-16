#include <string>
#include <array>

#include "catch2/catch.hpp"
#include "shared/utils/strings.h"

using namespace std::literals;
using namespace pbr::shared::utils;

/*********************************************
 * trim
 ********************************************/

TEST_CASE("trim - empty string - returns empty string", "[shared/utils/strings]") {
    auto s = "";
    auto result = trim(s);

    REQUIRE(result.empty());
}

TEST_CASE("trim - whitespace string - returns empty string", "[shared/utils/strings]") {
    auto s = "     ";
    auto result = trim(s);

    REQUIRE(result.empty());
}

TEST_CASE("trim - string with whitespace at beginning - returns trimmed string", "[shared/utils/strings]") {
    auto s = "  my text";
    auto result = trim(s);

    auto expected = "my text";
    REQUIRE(result == expected);
}

TEST_CASE("trim - string with whitespace at end - returns trimmed string", "[shared/utils/strings]") {
    auto s = "my text   ";
    auto result = trim(s);

    auto expected = "my text";
    REQUIRE(result == expected);
}

TEST_CASE("trim - string with whitespace at beginning and end - returns trimmed string", "[shared/utils/strings]") {
    auto s = "    my text   ";
    auto result = trim(s);

    auto expected = "my text";
    REQUIRE(result == expected);
}

TEST_CASE("trim - custom character - trims custom character", "[shared/utils/strings]") {
    auto s = "0000text00000";
    auto result = trim(s, '0');

    auto expected = "text";
    REQUIRE(result == expected);
}

/*********************************************
 * ltrim
 ********************************************/

TEST_CASE("ltrim - empty string - returns empty string", "[shared/utils/strings]") {
    auto s = "";
    auto result = ltrim(s);

    REQUIRE(result.empty());
}

TEST_CASE("ltrim - whitespace string - returns empty string", "[shared/utils/strings]") {
    auto s = "     ";
    auto result = ltrim(s);

    REQUIRE(result.empty());
}

TEST_CASE("ltrim - string with whitespace at beginning and end - returns left trimmed string", "[shared/utils/strings]") {
    auto s = "    my text   ";
    auto result = ltrim(s);

    auto expected = "my text   ";
    REQUIRE(result == expected);
}

TEST_CASE("ltrim - custom character - trims custom character", "[shared/utils/strings]") {
    auto s = "00000text";
    auto result = ltrim(s, '0');

    auto expected = "text";
    REQUIRE(result == expected);
}

/*********************************************
 * rtrim
 ********************************************/

TEST_CASE("rtrim - empty string - returns empty string", "[shared/utils/strings]") {
    auto s = "";
    auto result = rtrim(s);

    REQUIRE(result.empty());
}

TEST_CASE("rtrim - whitespace string - returns empty string", "[shared/utils/strings]") {
    auto s = "     ";
    auto result = rtrim(s);

    REQUIRE(result.empty());
}

TEST_CASE("rtrim - string with whitespace at beginning and end - returns right trimmed string", "[shared/utils/strings]") {
    auto s = "    my text   ";
    auto result = rtrim(s);

    auto expected = "    my text";
    REQUIRE(result == expected);
}

TEST_CASE("rtrim - custom character - trims custom character", "[shared/utils/strings]") {
    auto s = "text00000";
    auto result = rtrim(s, '0');

    auto expected = "text";
    REQUIRE(result == expected);
}

/*********************************************
 * to_lower
 ********************************************/

TEST_CASE("to_lower - empty string - returns empty string", "[shared/utils/strings]") {
    auto s = "";
    auto result = to_lower(s);

    REQUIRE(result.empty());
}

TEST_CASE("to_lower - string with uppercase letters - returns lowercase string", "[shared/utils/strings]") {
    auto s = " My StRiNG !! ";
    auto result = to_lower(s);

    auto expected = " my string !! ";
    REQUIRE(result == expected);
}

/*********************************************
 * to_upper
 ********************************************/

TEST_CASE("to_upper - empty string - returns empty string", "[shared/utils/strings]") {
    auto s = "";
    auto result = to_upper(s);

    REQUIRE(result.empty());
}

TEST_CASE("to_upper - string with lowercase letters - returns uppercase string", "[shared/utils/strings]") {
    auto s = " My StRiNG !! ";
    auto result = to_upper(s);

    auto expected = " MY STRING !! ";
    REQUIRE(result == expected);
}

/*********************************************
 * to_array
 ********************************************/

TEST_CASE("to_array - valid string - returns array", "[shared/utils/strings]") {
    constexpr auto s = "my string";
    auto result = to_array<9>(s);

    auto expected = std::array<uint8_t, 9>({
        'm',
        'y',
        ' ',
        's',
        't',
        'r',
        'i',
        'n',
        'g',
    });

    REQUIRE(result == expected);
}

TEST_CASE("to_array - string to short - returns array with backslash 0 as remaining characters", "[shared/utils/strings]") {
    constexpr auto s = "my str";
    auto result = to_array<9>(s);

    auto expected = std::array<uint8_t, 9>({
        'm',
        'y',
        ' ',
        's',
        't',
        'r',
        '\0',
        '\0',
        '\0',
    });

    REQUIRE(result == expected);
}

TEST_CASE("to_array - string to long - returns array of the specified length", "[shared/utils/strings]") {
    constexpr auto s = "my string my string my string";
    auto result = to_array<11>(s);

    auto expected = std::array<uint8_t, 11>({
        'm',
        'y',
        ' ',
        's',
        't',
        'r',
        'i',
        'n',
        'g',
        ' ',
        'm',
    });

    REQUIRE(result == expected);
}

/*********************************************
 * from_array
 ********************************************/

TEST_CASE("from_array - valid array - returns string", "[shared/utils/strings]") {
    constexpr auto a = std::array<uint8_t, 9>({
    'm',
    'y',
    ' ',
    's',
    't',
    'r',
    'i',
    'n',
    'g',
    });

    auto result = from_array<9>(a);

    constexpr auto expected = "my string";

    REQUIRE(result == expected);
}

/*********************************************
 * split
 ********************************************/

TEST_CASE("split - empty divisor - returns empty list", "[shared/utils/strings]") {
    constexpr auto divisor = "";
    constexpr auto s = "mykey=myvalue";

    auto result = split(divisor, s);

    std::vector<std::string> expected;

    REQUIRE(result == expected);
}

TEST_CASE("split - empty string to split - returns empty list", "[shared/utils/strings]") {
    constexpr auto divisor = "=";
    constexpr auto s = "";

    auto result = split(divisor, s);

    std::vector<std::string> expected;

    REQUIRE(result == expected);
}

TEST_CASE("split - single character - returns text", "[shared/utils/strings]") {
    constexpr auto divisor = "=";
    constexpr auto s = "a";

    auto result = split(divisor, s);

    std::vector<std::string> expected {
        s,
    };

    REQUIRE(result == expected);
}

TEST_CASE("split - no divisor - returns text", "[shared/utils/strings]") {
    constexpr auto divisor = "=";
    constexpr auto s = "mykey&myvalue";

    auto result = split(divisor, s);

    std::vector<std::string> expected {
        s,
    };

    REQUIRE(result == expected);
}

TEST_CASE("split - two parts - returns results list", "[shared/utils/strings]") {
    constexpr auto divisor = "=";
    constexpr auto s = "mykey=myvalue";

    auto result = split(divisor, s);

    std::vector<std::string> expected {
        "mykey",
        "myvalue",
    };

    REQUIRE(result == expected);
}

TEST_CASE("split - 5 parts - returns results list", "[shared/utils/strings]") {
    constexpr auto divisor = "=";
    constexpr auto s = "mykey=myvalue=1=234=56789";

    auto result = split(divisor, s);

    std::vector<std::string> expected {
        "mykey",
        "myvalue",
        "1",
        "234",
        "56789",
    };

    REQUIRE(result == expected);
}

TEST_CASE("split - multi-character delimiter - returns results list", "[shared/utils/strings]") {
    constexpr auto divisor = "<>";
    constexpr auto s = "mykey<>myvalue<>1<>234<>56789";

    auto result = split(divisor, s);

    std::vector<std::string> expected {
        "mykey",
        "myvalue",
        "1",
        "234",
        "56789",
    };

    REQUIRE(result == expected);
}

/*********************************************
 * starts_with
 ********************************************/

TEST_CASE("starts_with - empty string to check - returns false", "[shared/utils/strings]") {
    constexpr auto string_to_check = "";
    constexpr auto starts = "start part";

    auto result = starts_with(string_to_check, starts);

    REQUIRE_FALSE(result);
}

TEST_CASE("starts_with - empty value to check for - returns false", "[shared/utils/strings]") {
    constexpr auto string_to_check = "start part of this string";
    constexpr auto starts = "";

    auto result = starts_with(string_to_check, starts);

    REQUIRE_FALSE(result);
}

TEST_CASE("starts_with - value to check for longer than string to check - returns false", "[shared/utils/strings]") {
    constexpr auto string_to_check = "string";
    constexpr auto starts = "long value to check for";

    auto result = starts_with(string_to_check, starts);

    REQUIRE_FALSE(result);
}

TEST_CASE("starts_with - string starts with value to check for - returns true", "[shared/utils/strings]") {
    constexpr auto string_to_check = "start part of this string";
    constexpr auto starts = "start part";

    auto result = starts_with(string_to_check, starts);

    REQUIRE(result);
}

TEST_CASE("starts_with - string does not start with value to check for - returns true", "[shared/utils/strings]") {
    constexpr auto string_to_check = "start part of this string";
    constexpr auto starts = "abcd";

    auto result = starts_with(string_to_check, starts);

    REQUIRE_FALSE(result);
}

/*********************************************
 * ends_with
 ********************************************/

TEST_CASE("ends_with - empty string to check - returns false", "[shared/utils/strings]") {
    constexpr auto string_to_check = "";
    constexpr auto ends = "end part";

    auto result = ends_with(string_to_check, ends);

    REQUIRE_FALSE(result);
}

TEST_CASE("ends_with - empty value to check for - returns false", "[shared/utils/strings]") {
    constexpr auto string_to_check = "start part of this string";
    constexpr auto ends = "";

    auto result = ends_with(string_to_check, ends);

    REQUIRE_FALSE(result);
}

TEST_CASE("ends_with - value to check for longer than string to check - returns false", "[shared/utils/strings]") {
    constexpr auto string_to_check = "string";
    constexpr auto ends = "long value to check for";

    auto result = ends_with(string_to_check, ends);

    REQUIRE_FALSE(result);
}

TEST_CASE("ends_with - string ends with value to check for - returns true", "[shared/utils/strings]") {
    constexpr auto string_to_check = "this string has an end part";
    constexpr auto ends = "end part";

    auto result = ends_with(string_to_check, ends);

    REQUIRE(result);
}

TEST_CASE("ends_with - string does not end with value to check for - returns true", "[shared/utils/strings]") {
    constexpr auto string_to_check = "this string has an end part";
    constexpr auto ends = "abcd";

    auto result = ends_with(string_to_check, ends);

    REQUIRE_FALSE(result);
}

/*********************************************
 * to_int
 ********************************************/

TEST_CASE("to_int - empty string - returns empty", "[shared/utils/strings]") {
    auto s = "";
    auto result = to_int(s);

    REQUIRE_FALSE(result);
}

TEST_CASE("to_int - invalid string - returns empty", "[shared/utils/strings]") {
    auto s = "invalid";
    auto result = to_int(s);

    REQUIRE_FALSE(result);
}

TEST_CASE("to_int - valid string - returns correct results", "[shared/utils/strings]") {
    std::vector<std::tuple<std::string, int>> values {
        { "0", 0 },
        { "1", 1 },
        { "-1", -1 },
        { "10", 10 },
        { "999", 999 },
        { "1234567890", 1234567890 },
    };

    for (const auto& [value, expected] : values) {
        auto result = to_int(value);

        REQUIRE(result);
        REQUIRE(*result == expected);
    }
}

/*********************************************
 * to_float
 ********************************************/

TEST_CASE("to_float - empty string - returns empty", "[shared/utils/strings]") {
    auto s = "";
    auto result = to_float(s);

    REQUIRE_FALSE(result);
}

TEST_CASE("to_float - invalid string - returns empty", "[shared/utils/strings]") {
    auto s = "invalid";
    auto result = to_float(s);

    REQUIRE_FALSE(result);
}

TEST_CASE("to_float - valid string - returns correct results", "[shared/utils/strings]") {
    std::vector<std::tuple<std::string, float>> values {
        { "0.0", 0.0f },
        { "1.1", 1.1f },
        { "-1.1", -1.1f },
        { "10.01", 10.01f },
        { "999.999", 999.999f },
        { "123.4567890", 123.4567890f },
    };

    for (const auto& [value, expected] : values) {
        auto result = to_float(value);

        REQUIRE(result);
        REQUIRE(*result == expected);
    }
}
