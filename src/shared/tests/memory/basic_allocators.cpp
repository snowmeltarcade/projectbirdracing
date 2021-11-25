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

//////////
/// get_number_of_allocated_bytes
//////////

TEST_CASE("get_number_of_allocated_bytes - returns number of allocated bytes", "[shared/memory]") {
    auto start = get_number_of_allocated_bytes();

    auto size = 10;
    auto ptr = new int[size];

    auto end = get_number_of_allocated_bytes();

    auto result = end - start;

    REQUIRE(sizeof(int) * size == result);

    delete[] ptr;
}
