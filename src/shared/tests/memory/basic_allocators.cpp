#include "catch2/catch.hpp"

#include "shared/memory/basic_allocators.h"

using namespace pbr::shared::memory;

//////////
/// new
//////////

TEST_CASE("new - does not return null", "[shared/memory]") {
    auto result = new int;

    REQUIRE(result);

    delete result;
}

TEST_CASE("new - accumulates number of bytes allocated", "[shared/memory]") {
    auto before = get_number_of_allocated_bytes();

    auto ptr = new int;

    auto after = get_number_of_allocated_bytes();

    auto result = after - before;
    auto expected = sizeof(int);

    REQUIRE(result == expected);

    delete ptr;
}

//////////
/// delete
//////////

TEST_CASE("delete - decreases number of bytes allocated", "[shared/memory]") {
    auto expected = get_number_of_allocated_bytes();

    auto ptr = new int;

    auto after = get_number_of_allocated_bytes();
    REQUIRE(after > expected);

    delete ptr;

    auto result = get_number_of_allocated_bytes();

    REQUIRE(result == expected);
}
