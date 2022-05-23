#include "catch2/catch.hpp"
#include "test_utils.h"
#include "shared/entity_component_system/repository.h"

using namespace pbr::shared;
using namespace pbr::shared::entity_component_system;
using namespace pbr::shared::entity_component_system::components;

//////////
/// add_camera
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

TEST_CASE("add_camera - remove entity then add camera - adds camera into correct index", "[entity_component_system/repository]") {
    repository repository;

    camera camera1 {
        "camera name1",
        1,
        apis::graphics::render_systems::_2d,
        2,
    };

    camera camera2 {
        "camera name2",
        2,
        apis::graphics::render_systems::_3d,
        3,
    };

    location_3d location1 {
        1.0f,
        2.0f,
        3.0f,
    };

    location_3d location2 {
        2.0f,
        3.0f,
        4.0f,
    };

    [[maybe_unused]] auto id1 = repository.add_camera(camera1, location1);
    auto id2 = repository.add_camera(camera1, location1);
    [[maybe_unused]] auto id3 = repository.add_camera(camera1, location1);

    repository.remove_entity(id2);

    [[maybe_unused]] auto id4 = repository.add_camera(camera2, location2);

    // the 2nd id should be replaced by the 4th insert
    auto result_camera = repository.get<components::camera>(id2);
    auto result_location = repository.get<location_3d>(id2);

    REQUIRE(result_camera == camera2);
    REQUIRE(result_location == location2);
}

//////////
/// remove
//////////

TEST_CASE("remove - remove entity - resets data", "[entity_component_system/repository]") {
    repository repository;

    camera camera {
        "camera name1",
        1,
        apis::graphics::render_systems::_2d,
        2,
    };

    location_3d location {
        1.0f,
        2.0f,
        3.0f,
    };

    auto id = repository.add_camera(camera, location);

    repository.remove_entity(id);

    auto result_camera = repository.get<components::camera>(id);
    auto result_location = repository.get<location_3d>(id);

    REQUIRE(result_camera == components::camera{});
    REQUIRE(result_location == location_3d{});
}

//////////
/// get
//////////

TEST_CASE("get - gets camera - returns camera", "[entity_component_system/repository]") {
    repository repository;

    camera camera {
        "camera name1",
        1,
        apis::graphics::render_systems::_2d,
        2,
    };

    auto id = repository.add_camera(camera, {});

    auto result = repository.get<components::camera>(id);

    REQUIRE(result == camera);
}

TEST_CASE("get - gets location_3d - returns location_3d", "[entity_component_system/repository]") {
    repository repository;

    location_3d location {
        1.0f,
        2.0f,
        3.0f,
    };

    auto id = repository.add_camera({}, location);

    auto result = repository.get<location_3d>(id);

    REQUIRE(result == location);
}
