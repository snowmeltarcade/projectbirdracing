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

TEST_CASE("bytes constructor int - set value - sets value", "[shared/memory]") {
    auto expected = 1024u;

    bytes b(expected);

    auto result = b.get_value();

    REQUIRE(result == expected);
}

//////////
/// constructor kilobytes
//////////

TEST_CASE("bytes constructor kilobytes - set value - sets value", "[shared/memory]") {
    kilobytes kb(2);
    auto expected = 1024u * kb.get_value();

    bytes b(kb);

    auto result = b.get_value();

    REQUIRE(result == expected);
}

//////////
/// constructor megabytes
//////////

TEST_CASE("bytes constructor megabytes - set value - sets value", "[shared/memory]") {
    megabytes mb(2);
    auto expected = 1024u * 1024u * mb.get_value();

    bytes b(mb);

    auto result = b.get_value();

    REQUIRE(result == expected);
}

//////////
/// constructor gigabytes
//////////

TEST_CASE("bytes constructor gigabytes - set value - sets value", "[shared/memory]") {
    gigabytes gb(2);
    auto expected = 1024u * 1024u * 1024u * gb.get_value();

    bytes b(gb);

    auto result = b.get_value();

    REQUIRE(result == expected);
}

//////////
/// <=>
//////////

TEST_CASE("bytes <=> - compares - valid", "[shared/memory]") {
    bytes low(100);
    bytes equals_low(low.get_value());
    bytes high(200);

    REQUIRE(low < high);
    REQUIRE_FALSE(low > high);
    REQUIRE(low == equals_low);
    REQUIRE(low != high);
}

//////////
/// get_value
//////////

TEST_CASE("bytes get_value - gets value", "[shared/memory]") {
    auto expected = 100u;

    bytes b(expected);

    auto result = b.get_value();

    REQUIRE(result == expected);
}

//////////
/// operator +
//////////

TEST_CASE("bytes operator + - adds value - returns correct value", "[shared/memory]") {
    auto left = 10;
    auto right = 25;

    auto result = bytes(left) + bytes(right);

    auto expected = bytes(left + right);

    REQUIRE(result == expected);
}

//////////
/// operator -
//////////

TEST_CASE("bytes operator - - subtracts value - returns correct value", "[shared/memory]") {
    auto left = 10;
    auto right = 25;

    auto result = bytes(left) - bytes(right);

    auto expected = bytes(left - right);

    REQUIRE(result == expected);
}
