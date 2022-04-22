#include "catch2/catch.hpp"
#include "shared/resource/resource_manager.h"

#include <memory>

using namespace pbr::shared;
using namespace pbr::shared::resource;

class test_resource_manager : public resource_manager<int> {
public:
    int load_call_count {0u};
    std::shared_ptr<int> load(const std::filesystem::path&) noexcept override {
        ++load_call_count;
        return {};
    }
};

//////////
/// get
//////////

TEST_CASE("get - item not loaded - loads item", "[shared/resource/resource_manager]") {
    test_resource_manager manager;

    auto _ = manager.get("name");

    REQUIRE(manager.load_call_count == 1);
}

TEST_CASE("get - item loaded - loads item only once", "[shared/resource/resource_manager]") {
    test_resource_manager manager;

    auto _ = manager.get("name");
    _ = manager.get("name");
    _ = manager.get("name");

    REQUIRE(manager.load_call_count == 1);
}
