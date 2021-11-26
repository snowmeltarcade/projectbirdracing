#include "catch2/catch.hpp"
#include "shared/apis/datetime/datetime_manager.h"

#include <regex>

using namespace pbr::shared::apis::datetime;

//////////
/// get_date_as_utc_string
//////////

TEST_CASE("get_date_as_utc_string - returns valid date string", "[shared/apis/datetime]") {
    datetime_manager manager;

    auto value = manager.get_date_as_utc_string();

    std::regex r(R"regex(\d\d\d\d-\d\d-\d\d)regex");

    auto result = std::regex_match(value, r);

    REQUIRE(result);
}

//////////
/// get_time_as_utc_string
//////////

TEST_CASE("get_time_as_utc_string - returns valid time string", "[shared/apis/datetime]") {
    datetime_manager manager;

    auto value = manager.get_time_as_utc_string();

    std::regex r(R"regex(\d\d-\d\d-\d\d)regex");

    auto result = std::regex_match(value, r);

    REQUIRE(result);
}

//////////
/// get_date_time_as_utc_string
//////////

TEST_CASE("get_date_time_as_utc_string - returns valid date time string", "[shared/apis/datetime]") {
    datetime_manager manager;

    auto value = manager.get_date_time_as_utc_string();

    std::regex r(R"regex(\d\d\d\d-\d\d-\d\d \d\d-\d\d-\d\d)regex");

    auto result = std::regex_match(value, r);

    REQUIRE(result);
}
