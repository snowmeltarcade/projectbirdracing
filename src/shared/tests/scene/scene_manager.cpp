#include "catch2/catch.hpp"
#include "shared/scene/scene_manager.h"
#include "shared/apis/datetime/datetime_manager.h"
#include "shared/apis/logging/log_manager.h"

#include <memory>

using namespace pbr::shared;
using namespace pbr::shared::scene;

scene_manager create_scene_manager() {
    auto datetime_manager = std::make_shared<apis::datetime::datetime_manager>();
    auto log_manager = std::make_shared<apis::logging::log_manager>(datetime_manager);

    scene_manager sm(log_manager);
    return sm;
}

//////////
/// initialize
//////////

TEST_CASE("d", "[shared/scene]") {
    REQUIRE(true);
}
