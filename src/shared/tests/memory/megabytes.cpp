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

TEST_CASE("megabytes constructor int - set value - sets value", "[shared/memory]") {
    auto expected = 1024u;

    megabytes b(expected);

    auto result = b.get_value();

    REQUIRE(result == expected);
}

//////////
/// constructor bytes
//////////

TEST_CASE("megabytes constructor bytes - set value - sets value", "[shared/memory]") {
    bytes b(2048 * 1024);
    auto expected = b.get_value() / 1024u / 1024u;

    megabytes mb(b);

    auto result = mb.get_value();

    REQUIRE(result == expected);
}

//////////
/// constructor kilobytes
//////////

TEST_CASE("megabytes constructor kilobytes - set value - sets value", "[shared/memory]") {
    kilobytes kb(2048);
    auto expected = kb.get_value() / 1024u;

    megabytes mb(kb);

    auto result = mb.get_value();

    REQUIRE(result == expected);
}

//////////
/// constructor gigabytes
//////////

TEST_CASE("megabytes constructor gigabytes - set value - sets value", "[shared/memory]") {
    gigabytes gb(2);
    auto expected = 1024u * gb.get_value();

    megabytes mb(gb);

    auto result = mb.get_value();

    REQUIRE(result == expected);
}

//////////
/// <=>
//////////

TEST_CASE("megabytes <=> - compares - valid", "[shared/memory]") {
    megabytes low(100);
    megabytes equals_low(low.get_value());
    megabytes high(200);

    REQUIRE(low < high);
    REQUIRE_FALSE(low > high);
    REQUIRE(low == equals_low);
    REQUIRE(low != high);
}

//////////
/// get_value
//////////

TEST_CASE("megabytes get_value - gets value", "[shared/memory]") {
    auto expected = 100u;

    megabytes b(expected);

    auto result = b.get_value();

    REQUIRE(result == expected);
}
