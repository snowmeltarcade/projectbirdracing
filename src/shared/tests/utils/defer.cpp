#include "catch2/catch.hpp"
#include "shared/utils/defer.h"

using namespace std::literals;
using namespace pbr::shared::utils;

/*********************************************
 * destructor
 ********************************************/

TEST_CASE("destructor - calls callback", "[shared/utils/defer]") {
    auto result {false};

    {
        defer d { [&result]() { result = true; }};
    }

    REQUIRE(result);
}

TEST_CASE("destructor - destructor calls multiple times - callback called only once", "[shared/utils/defer]") {
    auto result {0u};

    {
        defer d { [&result]() { ++result; }};
        d.~defer();
        d.~defer();
        d.~defer();
    }

    REQUIRE(result == 1u);
}
