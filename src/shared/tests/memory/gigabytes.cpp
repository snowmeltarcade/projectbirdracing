#include <thread>
#include "catch2/catch.hpp"
#include "shared/memory/bytes.h"
#include "shared/memory/kilobytes.h"
#include "shared/memory/megabytes.h"
#include "shared/memory/gigabytes.h"

using namespace pbr::shared::memory;

//////////
/// constructor int
//////////

TEST_CASE("gigabytes constructor int - set value - sets value", "[shared/memory]") {
    auto expected = 1024u;

    gigabytes b(expected);

    auto result = b.get_value();

    REQUIRE(result == expected);
}

//////////
/// constructor bytes
//////////

TEST_CASE("gigabytes constructor bytes - set value - sets value", "[shared/memory]") {
    bytes b(2048 * 1024 * 1024);
    auto expected = b.get_value() / 1024u / 1024u / 1024u;

    gigabytes gb(b);

    auto result = gb.get_value();

    REQUIRE(result == expected);
}

//////////
/// constructor kilobytes
//////////

TEST_CASE("gigabytes constructor kilobytes - set value - sets value", "[shared/memory]") {
    kilobytes kb(2048 * 1024);
    auto expected = kb.get_value() / 1024u / 1024u;

    gigabytes gb(kb);

    auto result = gb.get_value();

    REQUIRE(result == expected);
}

//////////
/// constructor megabytes
//////////

TEST_CASE("gigabytes constructor megabytes - set value - sets value", "[shared/memory]") {
    megabytes mb(2048);
    auto expected = mb.get_value() / 1024;

    gigabytes gb(mb);

    auto result = gb.get_value();

    REQUIRE(result == expected);
}

//////////
/// <=>
//////////

TEST_CASE("gigabytes <=> - compares - valid", "[shared/memory]") {
    gigabytes low(100);
    gigabytes equals_low(low.get_value());
    gigabytes high(200);

    REQUIRE(low < high);
    REQUIRE_FALSE(low > high);
    REQUIRE(low == equals_low);
    REQUIRE(low != high);
}

//////////
/// get_value
//////////

TEST_CASE("gigabytes get_value - gets value", "[shared/memory]") {
    auto expected = 100u;

    gigabytes b(expected);

    auto result = b.get_value();

    REQUIRE(result == expected);
}
