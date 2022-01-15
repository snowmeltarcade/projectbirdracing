#include "catch2/catch.hpp"
#include "test_utils.h"
#include "shared/data/data_manager.h"
#include "shared/apis/datetime/datetime_manager.h"
#include "shared/apis/logging/log_manager.h"
#include "shared/apis/file/file_manager.h"

using namespace pbr::shared;
using namespace pbr::shared::data;

data_manager create_data_manager() {
    auto datetime_manager = std::make_shared<apis::datetime::datetime_manager>();
    auto log_manager = std::make_shared<apis::logging::log_manager>(datetime_manager);
    auto file_manager = std::make_shared<apis::file::file_manager>();

    auto data_path = get_test_data_file_path("data");

    data_manager dm(data_path,
                    file_manager,
                    log_manager);
    return dm;
}

//////////
/// read_settings
//////////

TEST_CASE("read_settings - invalid path - returns empty", "[shared/data]") {
    auto dm = create_data_manager();

    auto path = "invalid";

    auto result = dm.read_settings(path);

    REQUIRE_FALSE(result);
}

TEST_CASE("read_settings - invalid settings file - returns empty", "[shared/data]") {
    auto dm = create_data_manager();

    auto path = "invalid_settings";

    auto result = dm.read_settings(path);

    REQUIRE_FALSE(result);
}

TEST_CASE("read_settings - valid settings file - does not return empty", "[shared/data]") {
    auto dm = create_data_manager();

    auto path = "settings";

    auto result = dm.read_settings(path);

    REQUIRE(result);
}
