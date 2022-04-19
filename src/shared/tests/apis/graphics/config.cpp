#include "catch2/catch.hpp"
#include "shared/apis/graphics/config.h"
#include "shared/apis/datetime/datetime_manager.h"
#include "shared/apis/logging/log_manager.h"
#include "shared/apis/windowing/iwindow_manager.h"
#include "shared/apis/file/file_manager.h"
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

//////////
/// constructor
//////////

TEST_CASE("constructor - invalid api name - throws error", "[shared/apis/graphics/config]") {
    auto data_manager = create_data_manager();

    try {
        config(data_manager, g_log_manager, "config_invalid");
    } catch (...) {
        SUCCEED();
        return;
    }

    FAIL();
}

TEST_CASE("constructor - valid api name - sets correctly", "[shared/apis/graphics/config]") {
    auto data_manager = create_data_manager();

    std::vector<std::pair<std::filesystem::path, graphics::apis>> paths {
        { "config_opengl", graphics::apis::opengl, },
        { "config_vulkan", graphics::apis::vulkan, },
    };

    for (const auto& [path, expected] : paths) {
        config c(data_manager, g_log_manager, path);

        REQUIRE(c.api() == expected);
    }
}

//////////
/// api
//////////

TEST_CASE("api - returns api", "[shared/apis/graphics/config]") {
    auto data_manager = create_data_manager();

    config result(data_manager, g_log_manager, "config_opengl");

    REQUIRE(result.api() == graphics::apis::opengl);
}

//////////
/// set_api
//////////

TEST_CASE("set_api - sets api", "[shared/apis/graphics/config]") {
    auto data_manager = create_data_manager();

    config result(data_manager, g_log_manager, "config_opengl");

    auto expected = graphics::apis::vulkan;

    result.set_api(expected);

    REQUIRE(result.api() == expected);
}
