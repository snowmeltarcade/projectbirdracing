#include "catch2/catch.hpp"
#include "shared/game/game_manager.h"
#include "shared/apis/datetime/datetime_manager.h"
#include "shared/apis/logging/log_manager.h"
#include "shared/apis/windowing/window_manager.h"

#include <thread>
#include <memory>

using namespace pbr::shared;
using namespace pbr::shared::game;

game_manager create_game_manager() {
    auto datetime_manager = std::make_shared<apis::datetime::datetime_manager>();
    auto log_manager = std::make_shared<apis::logging::log_manager>(datetime_manager);
    auto window_manager = std::make_shared<apis::windowing::window_manager>(log_manager);

    game_manager gm(log_manager, window_manager);
    return gm;
}

//////////
/// initialize
//////////

TEST_CASE("initialize - successfully initializes game - returns true", "[shared/game]") {
    auto gm = create_game_manager();

    auto result = gm.initialize();

    REQUIRE(result);
}

//////////
/// run
//////////

TEST_CASE("run - runs one frame - returns true", "[shared/game]") {
    auto gm = create_game_manager();

    REQUIRE(gm.initialize());

    auto result = gm.run();

    REQUIRE(result);
}
