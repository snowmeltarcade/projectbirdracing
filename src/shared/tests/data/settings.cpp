#include "catch2/catch.hpp"
#include "test_utils.h"
#include "shared/data/settings.h"

using namespace pbr::shared;
using namespace pbr::shared::data;

//////////
/// add
//////////

TEST_CASE("add - valid key and value - adds setting", "[shared/data]") {
    std::string key1 = "my key1";
    std::string key2 = "my key2";
    std::string key3 = "my key3";

    std::string expected1 = "my value";

    auto value2 = 42;
    std::string expected2 = "42";

    auto value3 = true;
    std::string expected3 = "true";

    settings settings;

    settings.add(key1, expected1);
    settings.add(key2, value2);
    settings.add(key3, value3);

    auto result = settings.get(key1);
    REQUIRE(result == expected1);

    result = settings.get(key2);
    REQUIRE(result == expected2);

    result = settings.get(key3);
    REQUIRE(result == expected3);
}
