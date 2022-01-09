#include <string>
#include <array>

#include "catch2/catch.hpp"
#include "shared/utils/uri.h"

using namespace std::literals;
using namespace pbr::shared::utils;

/*********************************************
 * build_uri
 ********************************************/

TEST_CASE("build_uri - empty string - returns error", "[shared/utils/uri]") {
    auto s = "";

    auto result = build_uri(s);

    REQUIRE_FALSE(result);
}
