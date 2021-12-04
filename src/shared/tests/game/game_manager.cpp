#include "catch2/catch.hpp"
#include "shared/game/game_manager.h"
#include "shared/apis/datetime/datetime_manager.h"
#include "shared/apis/logging/log_manager.h"
#include "shared/apis/windowing/iwindow_manager.h"
#include "shared/apis/windowing/iconsole_window.h"
#include "shared/scene/iscene_manager.h"

#include <thread>
#include <memory>

using namespace pbr::shared;
using namespace pbr::shared::game;

class test_application_window : public apis::windowing::iapplication_window {

};

class test_window_manager : public apis::windowing::iwindow_manager {
public:
    bool initialize_called {false};
    bool initialize_result {true};

    bool initialize() noexcept override {
        this->initialize_called = true;
        return this->initialize_result;
    }

    std::shared_ptr<apis::windowing::iconsole_window> create_console_window() noexcept override {
        return {};
    }

    bool create_application_window_called {false};
    std::shared_ptr<apis::windowing::iapplication_window> create_application_window_result
        = std::make_shared<test_application_window>();

    std::shared_ptr<apis::windowing::iapplication_window> create_application_window() noexcept override {
        this->create_application_window_called = true;
        return this->create_application_window_result;
    }

    bool update_called {false};
    bool update_result {true};

    bool update() noexcept override {
        this->update_called = true;
        --this->frames_to_run_before_quit;
        return this->update_result;
    }

    int frames_to_run_before_quit = 1;

    bool should_quit() const noexcept override {
        return frames_to_run_before_quit < 0;
    }
};

class test_scene_manager : public scene::iscene_manager {
public:
    bool initialize_called {false};
    bool initialize_result {true};

    bool initialize() noexcept override {
        this->initialize_called = true;
        return this->initialize_result;
    }

    bool run_called {false};
    bool run_result {true};

    bool run() noexcept override {
        this->run_called = true;
        return this->run_result;
    }
};

std::shared_ptr<test_window_manager> g_window_manager;
std::shared_ptr<test_scene_manager> g_scene_manager;

game_manager create_game_manager() {
    auto datetime_manager = std::make_shared<apis::datetime::datetime_manager>();
    auto log_manager = std::make_shared<apis::logging::log_manager>(datetime_manager);

    g_window_manager = std::make_shared<test_window_manager>();

    g_scene_manager = std::make_shared<test_scene_manager>();

    game_manager gm(log_manager,
                    g_window_manager,
                    g_scene_manager);
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

TEST_CASE("initialize - creates application window", "[shared/game]") {
    auto gm = create_game_manager();

    REQUIRE(gm.initialize());

    auto result = g_window_manager->create_application_window_called;
    REQUIRE(result);
}

TEST_CASE("initialize - create application window fails - returns false", "[shared/game]") {
    auto gm = create_game_manager();

    g_window_manager->create_application_window_result = {};

    auto result = gm.initialize();
    REQUIRE_FALSE(result);
}

TEST_CASE("initialize - initializes scene manager", "[shared/game]") {
    auto gm = create_game_manager();

    REQUIRE(gm.initialize());

    auto result = g_scene_manager->initialize_called;
    REQUIRE(result);
}

TEST_CASE("initialize - initialize scene manager fails - returns false", "[shared/game]") {
    auto gm = create_game_manager();

    g_scene_manager->initialize_result = false;

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

TEST_CASE("run - runs one frame - updates window manager", "[shared/game]") {
    auto gm = create_game_manager();

    REQUIRE(gm.initialize());

    REQUIRE(gm.run());

    auto result = g_window_manager->update_called;
    REQUIRE(result);
}

TEST_CASE("run - update window manager returns false - returns false", "[shared/game]") {
    auto gm = create_game_manager();

    REQUIRE(gm.initialize());

    g_window_manager->update_result = false;

    auto result = gm.run();
    REQUIRE_FALSE(result);
}

TEST_CASE("run - window manager requests quit - returns true", "[shared/game]") {
    auto gm = create_game_manager();

    REQUIRE(gm.initialize());

    g_window_manager->frames_to_run_before_quit = 2;

    auto result = gm.run();
    REQUIRE(result);
}

TEST_CASE("run - runs one frame - runs scene manager", "[shared/game]") {
    auto gm = create_game_manager();

    REQUIRE(gm.initialize());

    REQUIRE(gm.run());

    auto result = g_scene_manager->run_called;
    REQUIRE(result);
}

TEST_CASE("run - running scene manager returns false - returns false", "[shared/game]") {
    auto gm = create_game_manager();

    REQUIRE(gm.initialize());

    g_scene_manager->run_result = false;

    auto result = gm.run();
    REQUIRE_FALSE(result);
}
