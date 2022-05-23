#include "catch2/catch.hpp"
#include "shared/scene/constructor.h"
#include "shared/apis/datetime/datetime_manager.h"
#include "shared/apis/logging/log_manager.h"
#include "shared/apis/file/file_manager.h"
#include "shared/data/data_manager.h"
#include "test_utils.h"

using namespace pbr::shared;
using namespace pbr::shared::data;
using namespace pbr::shared::scene;

class test_scene : public scene::scene_base {
public:
    test_scene(std::shared_ptr<apis::logging::ilog_manager> log_manager)
        : scene::scene_base(log_manager)
    {}

    scene_types get_scene_type() const noexcept override {
        return scene_types::loading;
    }

    bool load() noexcept override {
        return true;
    }

    bool run() noexcept override {
        return true;
    }

    bool should_quit() const noexcept override {
        // to speed up the tests, just quit as soon as we can
        return true;
    }

    std::filesystem::path get_data_file_name() const noexcept override {
        return "test";
    }
};

static auto g_datetime_manager = std::make_shared<apis::datetime::datetime_manager>();
static auto g_log_manager = std::make_shared<apis::logging::log_manager>(g_datetime_manager);

static std::shared_ptr<scene_base> create_scene() {
    return std::make_shared<test_scene>(g_log_manager);
}

static data_manager create_data_manager() {
    auto file_manager = std::make_shared<apis::file::file_manager>();

    auto data_path = get_test_data_file_path("scenes");

    data_manager dm(data_path,
                    file_manager,
                    g_log_manager);
    return dm;
}

static settings get_position(float x, float y, std::optional<float> z) {
    settings position;

    position.add("x", x);
    position.add("y", y);

    if (z) {
        position.add("z", *z);
    }

    return position;
}

static void add_camera(settings& settings_to_edit) {
    settings camera_settings;
    camera_settings.add("name", "camera name");
    camera_settings.add("layer", 1);
    camera_settings.add("render_target_name", "target name");
    camera_settings.add("system", "3d");
    camera_settings.add("position", get_position(1.0f, 2.0f, 3.0f));
    camera_settings.add("focus", get_position(2.0f, 3.0f, 4.0f));
    camera_settings.add("up", get_position(3.0f, 4.0f, 5.0f));

    settings_to_edit.add("cameras", camera_settings);
}

//////////
/// construct
//////////

TEST_CASE("construct - invalid camera - missing name - camera not added", "[shared/scene]") {
    auto scene = create_scene();
    auto data_manager = create_data_manager();

    settings settings;
    settings.add("name", "");

    add_camera(settings);

    constructor::construct(scene, settings, g_log_manager);
}

TEST_CASE("construct - valid cameras - add cameras", "[shared/scene]") {
    auto scene = create_scene();
    auto data_manager = create_data_manager();

    settings settings;
    add_camera(settings);

    constructor::construct(scene, settings, g_log_manager);


}
