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

    camera camera {
        "camera name",
        {},
        apis::graphics::render_systems::_3d,
        {},
    };

    location_3d location {
        0.0f,
        1.0f,
        2.0f,
    };

    auto id = repository.add_camera(camera, location);

    auto result_camera = repository.get<components::camera>(id);
    auto result_location = repository.get<location_3d>(id);

    REQUIRE(result_camera == camera);
    REQUIRE(result_location == location);
}
