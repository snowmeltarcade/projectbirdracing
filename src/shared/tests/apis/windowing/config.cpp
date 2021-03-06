#include "catch2/catch.hpp"
#include "shared/apis/windowing/config.h"
#include "shared/apis/windowing/resolution.h"
#include "shared/apis/datetime/datetime_manager.h"
#include "shared/apis/logging/log_manager.h"
#include "shared/apis/windowing/iwindow_manager.h"
#include "shared/apis/file/file_manager.h"
#include "shared/tests/test_utils.h"

using namespace pbr::shared;
using namespace pbr::shared::apis;
using namespace pbr::shared::data;
using namespace pbr::shared::apis::windowing;

static auto g_datetime_manager = std::make_shared<datetime::datetime_manager>();
static auto g_log_manager = std::make_shared<logging::log_manager>(g_datetime_manager);

static std::shared_ptr<data_manager> create_data_manager() {
    auto file_manager = std::make_shared<file::file_manager>();

    auto data_path = get_test_data_file_path("windowing");

    auto dm = std::make_shared<data_manager>(data_path,
                                             file_manager,
                                             g_log_manager);
    return dm;
}

//////////
/// constructor
//////////

TEST_CASE("constructor - valid resolutions - load resolutions", "[shared/apis/windowing/config]") {
    auto data_manager = create_data_manager();

    config c(data_manager, g_log_manager, "config_valid");

    std::vector<resolution> expected {
        { 1024, 768, false, },
        { 1280, 1024, true, },
    };

    REQUIRE(c.resolutions().size() == expected.size());

    for (auto i {0u}; i < expected.size(); ++i) {
        REQUIRE(c.resolutions()[i].width == expected[i].width);
        REQUIRE(c.resolutions()[i].height == expected[i].height);
        REQUIRE(c.resolutions()[i].fullscreen == expected[i].fullscreen);
    }
}

//////////
/// resolutions
//////////

TEST_CASE("resolutions - returns resolutions", "[shared/apis/windowing/config]") {
    auto data_manager = create_data_manager();

    config c(data_manager, g_log_manager, "config_valid");

    std::vector<resolution> expected {
        { 1024, 768, false, },
        { 1280, 1024, true, },
    };

    REQUIRE(c.resolutions().size() == expected.size());

    for (auto i {0u}; i < expected.size(); ++i) {
        REQUIRE(c.resolutions()[i].width == expected[i].width);
        REQUIRE(c.resolutions()[i].height == expected[i].height);
        REQUIRE(c.resolutions()[i].fullscreen == expected[i].fullscreen);
    }
}

//////////
/// default_resolution
//////////

TEST_CASE("default_resolution - returns default resolution", "[shared/apis/windowing/config]") {
    auto data_manager = create_data_manager();

    config c(data_manager, g_log_manager, "config_valid");

    resolution expected { 1280, 1024, true, };

    auto result = c.default_resolution();

    REQUIRE(result.width == expected.width);
    REQUIRE(result.height == expected.height);
    REQUIRE(result.fullscreen == expected.fullscreen);
}
