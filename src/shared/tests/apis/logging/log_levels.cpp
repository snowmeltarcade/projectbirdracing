#include "catch2/catch.hpp"
#include "shared/apis/logging/log_levels.h"

#include <vector>

using namespace pbr::shared::apis::logging;

//////////
/// to_string
//////////

TEST_CASE("to_string - endpoint - not empty", "[shared/apis/logging]") {
    // as this function returns data purely for presentation purposes, we will only
    // test that it is not empty
    std::vector<log_levels> levels {
        log_levels::info,
        log_levels::warning,
        log_levels::error,
        log_levels::fatal,
    };

    for (const auto& l : levels) {
        auto result = to_string(l);

        REQUIRE(!result.empty());
    }
}
