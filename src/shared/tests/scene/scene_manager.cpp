#include "catch2/catch.hpp"
#include "shared/scene/scene_manager.h"
#include "shared/apis/datetime/datetime_manager.h"
#include "shared/apis/logging/log_manager.h"

#include <memory>

using namespace pbr::shared;
using namespace pbr::shared::scene;

class test_scene_factory : public scene::iscene_factory {
public:
    [[nodiscard]]
    std::shared_ptr<scene_base> create_scene(scene_types) noexcept override {
        return {};
    }
};

std::shared_ptr<test_scene_factory> g_test_scene_factory;

scene_manager create_scene_manager() {
    auto datetime_manager = std::make_shared<apis::datetime::datetime_manager>();
    auto log_manager = std::make_shared<apis::logging::log_manager>(datetime_manager);

    g_test_scene_factory = std::make_shared<test_scene_factory>();

    scene_manager sm(g_test_scene_factory, log_manager);
    return sm;
}

//////////
/// initialize
//////////

TEST_CASE("d", "[shared/scene]") {
    REQUIRE(true);
}
