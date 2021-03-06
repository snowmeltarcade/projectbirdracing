#include "catch2/catch.hpp"
#include "shared/apis/graphics/graphics_manager_factory.h"
#include "shared/apis/datetime/datetime_manager.h"
#include "shared/apis/logging/log_manager.h"
#include "shared/apis/windowing/iwindow_manager.h"
#include "shared/apis/file/file_manager.h"
#include "shared/apis/graphics/vulkan/graphics_manager.h"
#include "shared/apis/graphics/opengl/graphics_manager.h"
#include "shared/tests/test_utils.h"

using namespace pbr::shared;
using namespace pbr::shared::apis;
using namespace pbr::shared::data;
using namespace pbr::shared::apis::graphics;

static auto g_datetime_manager = std::make_shared<datetime::datetime_manager>();
static auto g_log_manager = std::make_shared<logging::log_manager>(g_datetime_manager);

static std::shared_ptr<data_manager> create_data_manager() {
    auto file_manager = std::make_shared<file::file_manager>();

    auto data_path = get_test_data_file_path("graphics");

    auto dm = std::make_shared<data_manager>(data_path,
                                             file_manager,
                                             g_log_manager);
    return dm;
}

config create_config() {
    auto data_manager = create_data_manager();

    config c(data_manager, g_log_manager, "config_opengl");
    return c;
}

class dummy_window : public windowing::iwindow_manager {
    bool initialize() noexcept override {return true;}
    std::shared_ptr<windowing::iconsole_window> create_console_window() noexcept override {return {};}
    std::shared_ptr<windowing::iapplication_window> create_application_window() noexcept override {return {};}
    std::shared_ptr<windowing::iapplication_window> get_main_application_window() const noexcept override {return {};}
    bool update() noexcept override {return true;}
    bool should_quit() const noexcept override {return true;}
};

std::shared_ptr<windowing::iwindow_manager> create_window_manager() {
    return std::make_shared<dummy_window>();
}

//////////
/// create
//////////

TEST_CASE("create - invalid api name - returns empty", "[shared/apis/graphics/graphics_manager]") {
    auto data_manager = create_data_manager();
    auto window_manager = create_window_manager();
    auto config = create_config();

    config.set_api(static_cast<graphics::apis>(99999));

    auto result = graphics_manager_factory::create(config,
                                                   data_manager,
                                                   g_log_manager,
                                                   g_log_manager,
                                                   window_manager,
                                                   {},
                                                   {});

    REQUIRE_FALSE(result);
}

TEST_CASE("create - vulkan api name - returns vulkan manager", "[shared/apis/graphics/graphics_manager]") {
    auto data_manager = create_data_manager();
    auto window_manager = create_window_manager();
    auto config = create_config();

    config.set_api(graphics::apis::vulkan);

    auto result_manager = graphics_manager_factory::create(config,
                                                           data_manager,
                                                           g_log_manager,
                                                           g_log_manager,
                                                           window_manager,
                                                           {},
                                                           {});

    auto result = std::dynamic_pointer_cast<graphics::vulkan::graphics_manager>(result_manager);
    REQUIRE(result);
}

TEST_CASE("create - opengl api name - returns opengl manager", "[shared/apis/graphics/graphics_manager]") {
    auto data_manager = create_data_manager();
    auto window_manager = create_window_manager();
    auto config = create_config();

    config.set_api(graphics::apis::opengl);

    auto result_manager = graphics_manager_factory::create(config,
                                                           data_manager,
                                                           g_log_manager,
                                                           g_log_manager,
                                                           window_manager,
                                                           {},
                                                           {});

    auto result = std::dynamic_pointer_cast<graphics::opengl::graphics_manager>(result_manager);
    REQUIRE(result);
}