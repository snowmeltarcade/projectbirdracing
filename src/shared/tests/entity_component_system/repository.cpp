#include "catch2/catch.hpp"
#include "test_utils.h"
#include "shared/entity_component_system/repository.h"

using namespace pbr::shared;
using namespace pbr::shared::entity_component_system;
using namespace pbr::shared::entity_component_system::components;

//////////
/// add
//////////

TEST_CASE("add_camera - add camera - adds camera", "[entity_component_system/repository]") {
    repository repository;

    std::vector<camera> cameras {
        { "camera name1", {}, apis::graphics::render_systems::_2d, 1, },
        { "camera name2", 1, apis::graphics::render_systems::_3d, 3, },
        { "camera name3", 2, apis::graphics::render_systems::_2d, {}, },
    };

    std::vector<location_3d> locations {
        { 0.0f, 1.0f, 2.0f, },
        { 1.0f, 2.0f, 3.0f, },
        { 2.0f, 3.0f, 4.0f, },
    };

    std::vector<entity_id> ids;
    ids.reserve(cameras.size());

    for (auto i {0u}; i < cameras.size(); ++i) {
        ids.push_back(repository.add_camera(cameras[i], locations[i]));
    }

    for (auto i {0u}; i < cameras.size(); ++i) {
        auto id = ids[i];

        auto result_camera = repository.get<camera>(id);
        auto result_location = repository.get<location_3d>(id);

        REQUIRE(result_camera == cameras[i]);
        REQUIRE(result_location == locations[i]);
    }
}
