#include <thread>
#include "catch2/catch.hpp"
#include "shared/diagnostics/counter_set.h"

using namespace pbr::shared::diagnostics;

//////////
/// increment
//////////

TEST_CASE("increment - valid amount - increments counter", "[shared/diagnostics]") {
    counter_set c;

    auto key = "key";
    auto value = 10;

    c.increment_counter(key, value);
    c.increment_counter(key, value);

    auto result = c.get_counter(key);

    REQUIRE(result == value * 2);
}

//////////
/// decrement
//////////

TEST_CASE("decrement - valid amount - decrements counter", "[shared/diagnostics]") {
    counter_set c;

    auto key = "key";
    auto value = 10;

    c.decrement_counter(key, value);
    c.decrement_counter(key, value);

    auto result = c.get_counter(key);

    REQUIRE(result == -value * 2);
}

//////////
/// add_value_to_list
//////////

TEST_CASE("add_value_to_list - valid value - adds value to list", "[shared/diagnostics]") {
    counter_set c;

    auto key = "key";
    auto value = 10;
    auto value2 = 20;

    c.add_value_to_list(key, value);
    c.add_value_to_list(key, value2);

    auto result = c.get_values_for_counter_list(key);

    REQUIRE(result.size() == 2);
    REQUIRE(result[0] == value);
    REQUIRE(result[1] == value2);
}

//////////
/// get_counter
//////////

TEST_CASE("get_counter - returns counter", "[shared/diagnostics]") {
    counter_set c;

    auto key = "key";
    auto value = 10;

    c.increment_counter(key, value);

    auto result = c.get_counter(key);
    REQUIRE(result == value);

    c.increment_counter(key, value);

    result = c.get_counter(key);
    REQUIRE(result == value * 2);
}

//////////
/// get_values_for_counter_list
//////////

TEST_CASE("get_values_for_counter_list - returns values", "[shared/diagnostics]") {
    counter_set c;

    auto key = "key";
    auto value = 10;
    auto value2 = 20;

    c.add_value_to_list(key, value);

    auto result = c.get_values_for_counter_list(key);
    REQUIRE(result.size() == 1);
    REQUIRE(result[0] == value);

    c.add_value_to_list(key, value2);

    result = c.get_values_for_counter_list(key);
    REQUIRE(result.size() == 2);
    REQUIRE(result[0] == value);
    REQUIRE(result[1] == value2);
}

//////////
/// get_counter_for_duration
//////////

TEST_CASE("get_counter_for_duration - returns values", "[shared/diagnostics]") {
    counter_set c;

    auto key = "key";
    auto value = 10;

    auto now = std::chrono::system_clock::now();
    auto duration = std::chrono::milliseconds(500);

    auto expected {0};

    // ensure the last update time is set up correctly
    c.get_counter_for_duration(key, duration);

    while (std::chrono::system_clock::now() - now <= duration) {
        expected += value;

        c.increment_counter(key, value);

        auto result = c.get_counter_for_duration(key, duration);
        REQUIRE(result == expected);
    }

    // the duration has now passed
    auto passed_result = c.get_counter_for_duration(key, duration);
    REQUIRE(passed_result == 0);
}
