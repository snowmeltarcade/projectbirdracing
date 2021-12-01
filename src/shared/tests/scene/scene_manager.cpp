#include "catch2/catch.hpp"
#include "shared/scene/scene_manager.h"
#include "shared/apis/datetime/datetime_manager.h"
#include "shared/apis/logging/log_manager.h"

#include <memory>

using namespace pbr::shared;
using namespace pbr::shared::scene;

class test_scene : public scene::scene_base {
public:
    test_scene(std::shared_ptr<apis::logging::ilog_manager> log_manager)
        : scene::scene_base(log_manager)
    {}

    bool load_called {false};
    bool load_result {true};
    uint32_t load_call_count {0u};

    bool load() noexcept override {
        this->load_called = true;
        ++this->load_call_count;
        return this->load_result;
    }

    bool run_called {false};
    bool run_result {true};
    uint32_t run_call_count {0u};

    bool run() noexcept override {
        this->run_called = true;
        ++this->run_call_count;
        return this->run_result;
    }

    bool should_quit() const noexcept override {
        return false;
    }
};

std::shared_ptr<test_scene> g_test_scene_loading;
std::shared_ptr<test_scene> g_test_scene_queued;

scene_types test_scene_type_1 = static_cast<scene_types>(static_cast<int>(scene_types::loading) + 1);
scene_types test_scene_type_2 = static_cast<scene_types>(static_cast<int>(scene_types::loading) + 2);
scene_types test_scene_type_3 = static_cast<scene_types>(static_cast<int>(scene_types::loading) + 3);

class test_scene_factory : public scene::iscene_factory {
public:
    std::shared_ptr<scene_base> create_scene(scene_types type) noexcept override {
        if (type == scene_types::loading) {
            return g_test_scene_loading;
        }
        else if (type == test_scene_type_1 ||
                 type == test_scene_type_2 ||
                 type == test_scene_type_3) {
            return g_test_scene_queued;
        }

        return {};
    }
};

std::shared_ptr<test_scene_factory> g_test_scene_factory;

scene_manager create_scene_manager(scene_types scene_type = scene_types::loading) {
    auto datetime_manager = std::make_shared<apis::datetime::datetime_manager>();
    auto log_manager = std::make_shared<apis::logging::log_manager>(datetime_manager);

    g_test_scene_loading = std::make_shared<test_scene>(log_manager);
    g_test_scene_queued = std::make_shared<test_scene>(log_manager);
    g_test_scene_factory = std::make_shared<test_scene_factory>();

    scene_manager sm(g_test_scene_factory,
                     scene_type,
                     log_manager);
    return sm;
}

//////////
/// initialize
//////////

TEST_CASE("initialize - returns true", "[shared/scene]") {
    auto sm = create_scene_manager();

    auto result = sm.initialize();
    REQUIRE(result);
}

TEST_CASE("initialize - unknown loading scene type - returns false", "[shared/scene]") {
    auto sm = create_scene_manager((scene_types)99999);

    auto result = sm.initialize();
    REQUIRE_FALSE(result);
}

TEST_CASE("initialize - loads loading scene", "[shared/scene]") {
    auto sm = create_scene_manager();

    REQUIRE(sm.initialize());

    auto result = g_test_scene_loading->load_called;
    REQUIRE(result);
}

TEST_CASE("initialize - loads loading scene fails - returns false", "[shared/scene]") {
    auto sm = create_scene_manager();

    g_test_scene_loading->load_result = false;

    auto result = sm.initialize();
    REQUIRE_FALSE(result);
}

//////////
/// run
//////////

TEST_CASE("run - returns true", "[shared/scene]") {
    auto sm = create_scene_manager();

    REQUIRE(sm.initialize());

    auto result = sm.run();
    REQUIRE(result);
}

TEST_CASE("run - after initialization - runs loading scene", "[shared/scene]") {
    auto sm = create_scene_manager();

    REQUIRE(sm.initialize());

    REQUIRE(sm.run());

    auto result = g_test_scene_loading->run_called;
    REQUIRE(result);
}

TEST_CASE("run - running loading scene fails - returns false", "[shared/scene]") {
    auto sm = create_scene_manager();

    REQUIRE(sm.initialize());

    g_test_scene_loading->run_result = false;

    auto result = sm.run();
    REQUIRE_FALSE(result);
}

TEST_CASE("run - queue scenes - runs queued scenes", "[shared/scene]") {
    auto sm = create_scene_manager();

    REQUIRE(sm.initialize());

    std::vector<scene_types> types {
        test_scene_type_1,
        test_scene_type_2,
        test_scene_type_3,
    };

    REQUIRE(sm.queue_new_scenes(types));

    REQUIRE(sm.run());

    auto result = g_test_scene_queued->run_call_count;
    REQUIRE(result == types.size());
}

TEST_CASE("run - run queued scenes fails - returns false", "[shared/scene]") {
    auto sm = create_scene_manager();

    REQUIRE(sm.initialize());

    std::vector<scene_types> types {
        test_scene_type_1,
        test_scene_type_2,
        test_scene_type_3,
    };

    REQUIRE(sm.queue_new_scenes(types));

    g_test_scene_queued->run_result = false;

    auto result = sm.run();
    REQUIRE_FALSE(result);
}

//////////
/// queue_new_scenes
//////////

TEST_CASE("queue_new_scenes - empty types list - returns false", "[shared/scene]") {
    auto sm = create_scene_manager();

    REQUIRE(sm.initialize());

    std::vector<scene_types> types;

    auto result = sm.queue_new_scenes(types);
    REQUIRE_FALSE(result);
}

TEST_CASE("queue_new_scenes - invalid scene type - returns false", "[shared/scene]") {
    auto sm = create_scene_manager();

    REQUIRE(sm.initialize());

    std::vector<scene_types> types {
        (scene_types)99999,
    };

    auto result = sm.queue_new_scenes(types);
    REQUIRE_FALSE(result);
}

TEST_CASE("queue_new_scenes - valid types list - returns true", "[shared/scene]") {
    auto sm = create_scene_manager();

    REQUIRE(sm.initialize());

    std::vector<scene_types> types {
        test_scene_type_1,
        test_scene_type_2,
        test_scene_type_3,
    };

    REQUIRE(sm.run());
    REQUIRE(g_test_scene_loading->run_called);

    REQUIRE(sm.queue_new_scenes(types));

    // the test loading scene should no longer be loaded at this point
    g_test_scene_loading->run_called = false;
    REQUIRE(sm.run());
    REQUIRE_FALSE(g_test_scene_loading->run_called);
}

TEST_CASE("queue_new_scenes - valid types list - unloads previously loaded scenes", "[shared/scene]") {
    auto sm = create_scene_manager();

    REQUIRE(sm.initialize());

    std::vector<scene_types> types {
        test_scene_type_1,
        test_scene_type_2,
        test_scene_type_3,
    };

    auto result = sm.queue_new_scenes(types);
    REQUIRE(result);
}

TEST_CASE("queue_new_scenes - valid types list - loads queued scenes", "[shared/scene]") {
    auto sm = create_scene_manager();

    REQUIRE(sm.initialize());

    std::vector<scene_types> types {
        test_scene_type_1,
        test_scene_type_2,
        test_scene_type_3,
    };

    REQUIRE(sm.queue_new_scenes(types));

    auto result = g_test_scene_queued->load_call_count;
    REQUIRE(result == types.size());
}

TEST_CASE("queue_new_scenes - load queued scenes fails - returns false", "[shared/scene]") {
    auto sm = create_scene_manager();

    REQUIRE(sm.initialize());

    std::vector<scene_types> types {
        test_scene_type_1,
        test_scene_type_2,
        test_scene_type_3,
    };

    g_test_scene_queued->load_result = false;

    auto result = sm.queue_new_scenes(types);
    REQUIRE_FALSE(result);
}

// TODO: When a scene is loading after being queued, the loading scene should be run - possibly blocking the current thread?
// don't reload the loading screen if it is already loaded though, as the first time `queue_new_scenes` is called, the
// loading scene will already be loaded.
//
// After that - start implementing the loading scene, graphics, input, UI, world generation...
