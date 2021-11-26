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

TEST_CASE("kilobytes constructor int - set value - sets value", "[shared/memory]") {
    auto expected = 1024u;

    kilobytes kb(expected);

    auto result = kb.get_value();

    REQUIRE(result == expected);
}

//////////
/// constructor bytes
//////////

TEST_CASE("kilobytes constructor bytes - set value - sets value", "[shared/memory]") {
    bytes b(2048);
    auto expected = b.get_value() / 1024u;

    kilobytes kb(b);

    auto result = kb.get_value();

    REQUIRE(result == expected);
}

//////////
/// constructor megabytes
//////////

TEST_CASE("kilobytes constructor megabytes - set value - sets value", "[shared/memory]") {
    megabytes mb(2);
    auto expected = 1024u * mb.get_value();

    kilobytes kb(mb);

    auto result = kb.get_value();

    REQUIRE(result == expected);
}

//////////
/// constructor gigabytes
//////////

TEST_CASE("kilobytes constructor gigabytes - set value - sets value", "[shared/memory]") {
    gigabytes gb(2);
    auto expected = 1024u * 1024u * gb.get_value();

    kilobytes kb(gb);

    auto result = kb.get_value();

    REQUIRE(result == expected);
}

//////////
/// <=>
//////////

TEST_CASE("kilobytes <=> - compares - valid", "[shared/memory]") {
    kilobytes low(100);
    kilobytes equals_low(low.get_value());
    kilobytes high(200);

    REQUIRE(low < high);
    REQUIRE_FALSE(low > high);
    REQUIRE(low == equals_low);
    REQUIRE(low != high);
}

//////////
/// get_value
//////////

TEST_CASE("kilobytes get_value - gets value", "[shared/memory]") {
    auto expected = 100u;

    kilobytes b(expected);

    auto result = b.get_value();

    REQUIRE(result == expected);
}
