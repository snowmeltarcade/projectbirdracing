#include "catch2/catch.hpp"
#include "shared/game/game_manager.h"
#include "shared/apis/datetime/datetime_manager.h"
#include "shared/apis/logging/log_manager.h"
#include "shared/apis/windowing/iwindow_manager.h"
#include "shared/apis/windowing/iconsole_window.h"

#include <thread>
#include <memory>

using namespace pbr::shared;
using namespace pbr::shared::game;

class test_window_manager : public apis::windowing::iwindow_manager {
public:
    bool initialize() noexcept {
        this->initialize_called = true;
        return this->initialize_result;
    };

    std::shared_ptr<apis::windowing::iconsole_window> create_console_window() noexcept {
        return {};
    };

    bool initialize_called {false};
    bool initialize_result {true};
};

std::shared_ptr<test_window_manager> g_window_manager;

game_manager create_game_manager() {
    auto datetime_manager = std::make_shared<apis::datetime::datetime_manager>();
    auto log_manager = std::make_shared<apis::logging::log_manager>(datetime_manager);

    g_window_manager = std::make_shared<test_window_manager>();

    game_manager gm(log_manager, g_window_manager);
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

TEST_CASE("initialize - initializes window manager", "[shared/game]") {
    auto gm = create_game_manager();

    REQUIRE(gm.initialize());

    auto result = g_window_manager->initialize_called;
    REQUIRE(result);
}

TEST_CASE("initialize - initialize window manager fails - returns false", "[shared/game]") {
    auto gm = create_game_manager();

    g_window_manager->initialize_result = false;

    auto result = gm.initialize();
    REQUIRE_FALSE(result);
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
