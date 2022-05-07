#include "catch2/catch.hpp"
#include "shared/scene/scene_manager.h"
#include "shared/apis/datetime/datetime_manager.h"
#include "shared/apis/logging/log_manager.h"
#include "shared/apis/file/file_manager.h"
#include "shared/data/data_manager.h"
#include "test_utils.h"

#include <memory>
#include <thread>
#include <chrono>
#include <atomic>

using namespace pbr::shared;
using namespace pbr::shared::scene;

scene_types test_scene_type_1 = static_cast<scene_types>(static_cast<int>(scene_types::loading) + 1);
scene_types test_scene_type_2 = static_cast<scene_types>(static_cast<int>(scene_types::loading) + 2);
scene_types test_scene_type_3 = static_cast<scene_types>(static_cast<int>(scene_types::loading) + 3);

/// Records the order that the scenes loaded
std::vector<scene_types> g_scene_load_order;

std::atomic_bool g_have_all_scenes_loaded {false};

class test_scene : public scene::scene_base {
public:
    test_scene(std::shared_ptr<apis::logging::ilog_manager> log_manager, scene_types scene_type)
        : scene::scene_base(log_manager),
          _scene_type(scene_type)
    {}

    // setting up a test will set this specifically from the constructor
    scene_types _scene_type;

    scene_types get_scene_type() const noexcept override {
        return _scene_type;
    }

    bool load_called {false};
    bool load_result {true};
    uint32_t load_call_count {0u};

    bool load() noexcept override {
        this->load_called = true;
        ++this->load_call_count;

        if (this->_scene_type != scene_types::loading) {
            // loading often takes time...
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }

        g_scene_load_order.push_back(this->_scene_type);

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
        // to speed up the tests, just quit as soon as we can
        return true;
    }

    std::filesystem::path get_data_file_name() const noexcept override {
        return "test";
    }
};

std::shared_ptr<test_scene> g_test_scene_loading;
std::shared_ptr<test_scene> g_test_scene_1;
std::shared_ptr<test_scene> g_test_scene_2;
std::shared_ptr<test_scene> g_test_scene_3;

class test_scene_factory : public scene::iscene_factory {
public:
    std::shared_ptr<scene_base> create_scene(scene_types type) noexcept override {
        if (type == scene_types::loading) {
            return g_test_scene_loading;
        } else if (type == test_scene_type_1) {
            return g_test_scene_1;
        } else if (type == test_scene_type_2) {
            return g_test_scene_2;
        } else if (type == test_scene_type_3) {
            return g_test_scene_3;
        }

        return {};
    }

    std::vector<scene_types> get_next_scenes(const std::vector<std::shared_ptr<scene_base>>& scenes) noexcept override {
        if (scenes.empty()) {
            return { test_scene_type_1 };
        }

        auto type = scenes[0]->get_scene_type();

        if (type == test_scene_type_1) {
            return { test_scene_type_2 };
        }
        else if (type == test_scene_type_2) {
            return { test_scene_type_3 };
        }

        g_have_all_scenes_loaded = true;
        return {};
    }
};

std::shared_ptr<test_scene_factory> g_test_scene_factory;

static std::shared_ptr<data::data_manager> create_data_manager() {
    auto datetime_manager = std::make_shared<apis::datetime::datetime_manager>();
    auto log_manager = std::make_shared<apis::logging::log_manager>(datetime_manager);
    auto file_manager = std::make_shared<apis::file::file_manager>();

    auto data_path = get_test_data_file_path("scenes");

    auto dm = std::make_shared<data::data_manager>(data_path,
                                                   file_manager,
                                                   log_manager);
    return dm;
}

std::shared_ptr<scene_manager> create_scene_manager(scene_types scene_type = scene_types::loading) {
    auto datetime_manager = std::make_shared<apis::datetime::datetime_manager>();
    auto log_manager = std::make_shared<apis::logging::log_manager>(datetime_manager);

    auto data_manager = create_data_manager();

    g_test_scene_loading = std::make_shared<test_scene>(log_manager, scene_types::loading);
    g_test_scene_1 = std::make_shared<test_scene>(log_manager, test_scene_type_1);
    g_test_scene_2 = std::make_shared<test_scene>(log_manager, test_scene_type_2);
    g_test_scene_3 = std::make_shared<test_scene>(log_manager, test_scene_type_3);
    g_test_scene_factory = std::make_shared<test_scene_factory>();

    g_scene_load_order.clear();
    g_have_all_scenes_loaded = false;

    auto sm = std::make_shared<scene_manager>(g_test_scene_factory,
                                              scene_type,
                                              data_manager,
                                              log_manager);
    return sm;
}

//////////
/// run
//////////

TEST_CASE("run - returns true", "[shared/scene]") {
    auto sm = create_scene_manager();

    auto result = sm->run();
    REQUIRE(result);
}

TEST_CASE("run - unknown loading scene type - returns false", "[shared/scene]") {
    auto sm = create_scene_manager((scene_types)99999);

    auto result = sm->run();
    REQUIRE_FALSE(result);
}

TEST_CASE("run - after initialization - runs all scenes from scene factory", "[shared/scene]") {
    auto sm = create_scene_manager();

    // we're expecting the loading scene, then test scenes 1, 2 then 3 to load and run
    while (!g_have_all_scenes_loaded) {
        REQUIRE(sm->run());
    }

    // the 3 test scenes plus the 4 times the loading scene will have loaded
    auto expected_number_of_scenes {7u};

    REQUIRE(g_scene_load_order.size() == expected_number_of_scenes);
    REQUIRE(g_scene_load_order[0] == scene_types::loading);
    REQUIRE(g_scene_load_order[1] == test_scene_type_1);
    REQUIRE(g_scene_load_order[2] == scene_types::loading);
    REQUIRE(g_scene_load_order[3] == test_scene_type_2);
    REQUIRE(g_scene_load_order[4] == scene_types::loading);
    REQUIRE(g_scene_load_order[5] == test_scene_type_3);
    REQUIRE(g_scene_load_order[6] == scene_types::loading);

    REQUIRE(g_test_scene_loading->load_called);
    REQUIRE(g_test_scene_loading->run_called);

    REQUIRE(g_test_scene_1->load_called);
    REQUIRE(g_test_scene_1->run_called);

    REQUIRE(g_test_scene_2->load_called);
    REQUIRE(g_test_scene_2->run_called);

    REQUIRE(g_test_scene_3->load_called);
    REQUIRE(g_test_scene_3->run_called);
}

TEST_CASE("run - load loading scenes fails - returns false", "[shared/scene]") {
    auto sm = create_scene_manager();

    g_test_scene_loading->load_result = false;

    REQUIRE_FALSE(sm->run());
}

TEST_CASE("run - run loading scenes fails - returns false", "[shared/scene]") {
    auto sm = create_scene_manager();

    g_test_scene_loading->run_result = false;

    // keep running until we return false
    auto has_returned_false {false};

    while (!g_have_all_scenes_loaded) {
        if (!sm->run()) {
            has_returned_false = true;
            break;
        }
    }

    // false must have been returned before all the scenes have loaded and run
    REQUIRE_FALSE(g_have_all_scenes_loaded);
    REQUIRE(has_returned_false);
}

TEST_CASE("run - load test scene 1 fails - returns false", "[shared/scene]") {
    auto sm = create_scene_manager();

    g_test_scene_1->load_result = false;

    // keep running until we return false
    auto has_returned_false {false};

    while (!g_have_all_scenes_loaded) {
        if (!sm->run()) {
            has_returned_false = true;
            break;
        }
    }

    // false must have been returned before all the scenes have loaded and run
    REQUIRE_FALSE(g_have_all_scenes_loaded);
    REQUIRE(has_returned_false);
}

TEST_CASE("run - run test scene 1 fails - returns false", "[shared/scene]") {
    auto sm = create_scene_manager();

    g_test_scene_1->run_result = false;

    // keep running until we return false
    auto has_returned_false {false};

    while (!g_have_all_scenes_loaded) {
        if (!sm->run()) {
            has_returned_false = true;
            break;
        }
    }

    // false must have been returned before all the scenes have loaded and run
    REQUIRE_FALSE(g_have_all_scenes_loaded);
    REQUIRE(has_returned_false);
}

TEST_CASE("run - load test scene 2 fails - returns false", "[shared/scene]") {
    auto sm = create_scene_manager();

    g_test_scene_2->load_result = false;

    // keep running until we return false
    auto has_returned_false {false};

    while (!g_have_all_scenes_loaded) {
        if (!sm->run()) {
            has_returned_false = true;
            break;
        }
    }

    // false must have been returned before all the scenes have loaded and run
    REQUIRE_FALSE(g_have_all_scenes_loaded);
    REQUIRE(has_returned_false);
}

TEST_CASE("run - run test scene 2 fails - returns false", "[shared/scene]") {
    auto sm = create_scene_manager();

    g_test_scene_2->run_result = false;

    // keep running until we return false
    auto has_returned_false {false};

    while (!g_have_all_scenes_loaded) {
        if (!sm->run()) {
            has_returned_false = true;
            break;
        }
    }

    // false must have been returned before all the scenes have loaded and run
    REQUIRE_FALSE(g_have_all_scenes_loaded);
    REQUIRE(has_returned_false);
}

TEST_CASE("run - load test scene 3 fails - returns false", "[shared/scene]") {
    auto sm = create_scene_manager();

    g_test_scene_3->load_result = false;

    // keep running until we return false
    auto has_returned_false {false};

    while (!g_have_all_scenes_loaded) {
        if (!sm->run()) {
            has_returned_false = true;
            break;
        }
    }

    // false must have been returned before all the scenes have loaded and run
    REQUIRE_FALSE(g_have_all_scenes_loaded);
    REQUIRE(has_returned_false);
}

TEST_CASE("run - run test scene 3 fails - returns false", "[shared/scene]") {
    auto sm = create_scene_manager();

    g_test_scene_3->run_result = false;

    // keep running until we return false
    auto has_returned_false {false};

    while (!g_have_all_scenes_loaded) {
        if (!sm->run()) {
            has_returned_false = true;
            break;
        }
    }

    // false must have been returned before all the scenes have loaded and run
    REQUIRE_FALSE(g_have_all_scenes_loaded);
    REQUIRE(has_returned_false);
}
