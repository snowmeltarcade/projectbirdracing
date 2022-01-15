#include "catch2/catch.hpp"
#include "shared/utils/stringable.h"

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

    bool b {false};
    stringable_concept_test(b);

    std::string s;
    stringable_concept_test(s);
}
