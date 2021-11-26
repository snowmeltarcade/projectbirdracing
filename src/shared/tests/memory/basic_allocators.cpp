#include <thread>
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
    auto expected = bytes(sizeof(int) + sizeof(memory_block_header));

    REQUIRE(result == expected);

    delete ptr;
}

//////////
/// delete
//////////

TEST_CASE("delete - single element, decreases number of bytes allocated", "[shared/memory]") {
    auto expected = get_number_of_allocated_bytes();

    auto ptr = new int;

    auto after = get_number_of_allocated_bytes();
    REQUIRE(after > expected);

    delete ptr;

    auto result = get_number_of_allocated_bytes();

    REQUIRE(result == expected);
}

TEST_CASE("delete - array of elements, decreases number of bytes allocated", "[shared/memory]") {
    auto expected = get_number_of_allocated_bytes();

    auto size = 100;
    auto ptr = new int[size];

    auto after = get_number_of_allocated_bytes();
    REQUIRE(after > expected);

    delete[] ptr;

    auto result = get_number_of_allocated_bytes();

    REQUIRE(result == expected);
}

//////////
/// get_number_of_allocated_bytes
//////////

TEST_CASE("get_number_of_allocated_bytes - single thread - returns number of allocated bytes", "[shared/memory]") {
    auto start = get_number_of_allocated_bytes();

    auto array_size = 10;
    auto ptr = new int[array_size];

    auto end = get_number_of_allocated_bytes();

    auto result = end - start;

    auto expected = bytes((sizeof(int) * array_size) + sizeof(memory_block_header));

    REQUIRE(result == expected);

    delete[] ptr;
}

TEST_CASE("get_number_of_allocated_bytes - multiple threads - returns number of allocated bytes", "[shared/memory]") {
    auto array_size = 10;
    auto number_of_threads = 20;

    auto start = get_number_of_allocated_bytes();

    // do this in a block to free any `std` allocated memory
    {
        auto threads = new std::thread[number_of_threads];

        for (auto i {0}; i < number_of_threads; ++i)
        {
            std::thread t([array_size](){
                // allocate and forget...
                new int[array_size];
            });
            threads[i] = std::move(t);
        }

        for (auto i {0}; i < number_of_threads; ++i) {
            threads[i].join();
        }

        delete[] threads;
    }

    auto end = get_number_of_allocated_bytes();

    auto result = end - start;

    auto expected = bytes(((sizeof(int) * array_size) + sizeof(memory_block_header)) * number_of_threads);

    REQUIRE(result == expected);
}
