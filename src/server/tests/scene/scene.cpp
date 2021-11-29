#include "catch2/catch.hpp"
#include "scene/scene_factory.h"
#include "shared/scene/scenes/loading_scene.h"

#include <vector>

using namespace pbr;
using namespace pbr::server;

//////////
/// create_scene
//////////

TEST_CASE("create_scene - invalid type - returns nullptr", "[server/scene]") {
    auto invalid_type = (shared::scene::scene_types)99999;
    scene::scene_factory sf;

    auto result = sf.create_scene(invalid_type);

    REQUIRE_FALSE(result);
}

TEST_CASE("create_scene - loading scene type - returns loading scene type", "[server/scene]") {
    auto type = shared::scene::scene_types::loading;
    scene::scene_factory sf;

    auto created_type = sf.create_scene(type);

    auto result = std::dynamic_pointer_cast<shared::scene::scenes::loading_scene>(created_type);
    REQUIRE(result);
}
