#include "catch2/catch.hpp"
#include "shared/resource/item.h"

#include <memory>

using namespace pbr::shared;
using namespace pbr::shared::resource;

class test_item : public item<int> {
};

//////////
/// increase_reference
//////////

TEST_CASE("increase_reference - increases the reference", "[shared/resource/item]") {
    test_item test_item;

    auto current_references = test_item.references();

    test_item.increase_reference();

    auto result = test_item.references();

    REQUIRE(result == current_references + 1);
}

//////////
/// decrease_reference
//////////

TEST_CASE("decrease_reference - decreases the reference", "[shared/resource/item]") {
    test_item test_item;

    // we need to increase before we can decrease
    test_item.increase_reference();
    test_item.increase_reference();

    REQUIRE_FALSE(test_item.decrease_reference());

    auto result = test_item.references();

    REQUIRE(result == 1);
}

TEST_CASE("decrease_reference - returns true when all references are gone", "[shared/resource/item]") {
    test_item test_item;

    // we need to increase before we can decrease
    test_item.increase_reference();
    test_item.increase_reference();

    REQUIRE_FALSE(test_item.decrease_reference());
    REQUIRE(test_item.decrease_reference());
}
