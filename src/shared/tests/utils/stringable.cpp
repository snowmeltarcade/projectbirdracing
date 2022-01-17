#include "catch2/catch.hpp"
#include "shared/utils/stringable.h"
#include "shared/utils/strings.h"

#include <vector>
#include <tuple>

using namespace pbr::shared::utils;

/*********************************************
 * to_string (int)
 ********************************************/

TEST_CASE("to_string (int) - valid value - returns correct result", "[shared/utils/stringable]") {
    std::vector<std::pair<int, std::string>> values {
        { 0, "0" },
        { 11, "11" },
        { 123456789, "123456789" },
        { -123456789, "-123456789" },
    };

    for (const auto& [value, expected] : values) {
        auto result = to_string(value);

        REQUIRE(result == expected);
    }
}

/*********************************************
 * to_string (float)
 ********************************************/

TEST_CASE("to_string (float) - valid value - returns correct result", "[shared/utils/stringable]") {
    std::vector<std::pair<float, std::string>> values {
        { 0.0f, "0.0" },
        { 11.11f, "11.11" },
        { 123.4567f, "123.4567" },
        { -123.4567f, "-123.4567" },
    };

    for (const auto& [value, expected] : values) {
        auto result = to_string(value);

        // use `starts_with` over `==` as we do not know how many trailing 0's there may be
        // in the string
        INFO(result);
        REQUIRE(starts_with(result, expected));
    }
}

/*********************************************
 * to_string (bool)
 ********************************************/

TEST_CASE("to_string (bool) - valid value - returns correct result", "[shared/utils/stringable]") {
    std::vector<std::pair<bool, std::string>> values {
        { true, "true" },
        { false, "false" },
    };

    for (const auto& [value, expected] : values) {
        auto result = to_string(value);

        REQUIRE(result == expected);
    }
}

/*********************************************
 * stringable - concept
 ********************************************/

template <stringable T>
void stringable_concept_test(T) {}

TEST_CASE("stringable - constrains correct types", "[shared/utils/stringable]") {
    int i {0};
    stringable_concept_test(i);

    uint32_t ui {0u};
    stringable_concept_test(ui);

    float f {0.0f};
    stringable_concept_test(f);

    bool b {false};
    stringable_concept_test(b);
}
