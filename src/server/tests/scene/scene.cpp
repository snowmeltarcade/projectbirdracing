#include "catch2/catch.hpp"
#include "scene/scene_factory.h"
#include "shared/apis/datetime/datetime_manager.h"
#include "shared/apis/logging/log_manager.h"
#include "shared/scene/scenes/loading_scene.h"
#include "shared/scene/scenes/world_generation_scene.h"

#include <vector>

using namespace pbr;
using namespace pbr::server;

scene::scene_factory create_scene_factory() {
    auto datetime_manager = std::make_shared<shared::apis::datetime::datetime_manager>();
    auto log_manager = std::make_shared<shared::apis::logging::log_manager>(datetime_manager);

    scene::scene_factory sf(log_manager);

    return sf;
}

//////////
/// create_scene
//////////

TEST_CASE("create_scene - invalid type - returns nullptr", "[server/scene]") {
    auto invalid_type = (shared::scene::scene_types)99999;
    auto sf = create_scene_factory();

    auto result = sf.create_scene(invalid_type);

    REQUIRE_FALSE(result);
}

TEST_CASE("create_scene - loading scene type - returns loading scene type", "[server/scene]") {
    auto type = shared::scene::scene_types::loading;
    auto sf = create_scene_factory();

    auto created_type = sf.create_scene(type);

    auto result = std::dynamic_pointer_cast<shared::scene::scenes::loading_scene>(created_type);
    REQUIRE(result);
}

TEST_CASE("create_scene - world generation scene type - returns world generation scene type", "[server/scene]") {
    auto type = shared::scene::scene_types::world_generation;
    auto sf = create_scene_factory();

    auto created_type = sf.create_scene(type);

    auto result = std::dynamic_pointer_cast<shared::scene::scenes::world_generation_scene>(created_type);
    REQUIRE(result);
}
