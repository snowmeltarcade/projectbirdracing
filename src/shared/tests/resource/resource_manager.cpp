#include "catch2/catch.hpp"
#include "test_utils.h"
#include "shared/resource/resource_manager.h"
#include "shared/apis/datetime/datetime_manager.h"
#include "shared/apis/logging/log_manager.h"
#include "shared/apis/file/file_manager.h"

#include <memory>

using namespace pbr::shared;
using namespace pbr::shared::data;
using namespace pbr::shared::resource;

static auto g_datetime_manager = std::make_shared<apis::datetime::datetime_manager>();
static auto g_log_manager = std::make_shared<apis::logging::log_manager>(g_datetime_manager);

static std::shared_ptr<data_manager> create_data_manager() {
    auto file_manager = std::make_shared<apis::file::file_manager>();

    auto data_path = get_test_data_file_path("resources");

    auto dm = std::make_shared<data_manager>(data_path,
                                             file_manager,
                                             g_log_manager);
    return dm;
}

class test_resource_manager : public resource_manager<int> {
public:
    test_resource_manager(std::shared_ptr<data_manager> data_manager)
        : resource_manager(data_manager,
                           g_log_manager,
                           "resource_list") {
    }

    int load_call_count {0u};
    std::filesystem::path loaded_path;

    std::shared_ptr<int> load(const std::filesystem::path& path) noexcept override {
        ++load_call_count;
        loaded_path = path;
        return {};
    }
};

//////////
/// get
//////////

TEST_CASE("get - invalid name - returns nullptr", "[shared/resource/resource_manager]") {
    auto data_manager = create_data_manager();

    test_resource_manager manager(data_manager);

    auto result = manager.get("invalid name");

    REQUIRE_FALSE(result);
}

TEST_CASE("get - item not loaded - loads item", "[shared/resource/resource_manager]") {
    auto data_manager = create_data_manager();

    test_resource_manager manager(data_manager);

    auto _ = manager.get("name");

    REQUIRE(manager.load_call_count == 1);
}

TEST_CASE("get - item loaded - loads item only once", "[shared/resource/resource_manager]") {
    auto data_manager = create_data_manager();

    test_resource_manager manager(data_manager);

    auto _ = manager.get("name");
    _ = manager.get("name");
    _ = manager.get("name");

    REQUIRE(manager.load_call_count == 1);
}

TEST_CASE("get - item not loaded - passes correct path to load", "[shared/resource/resource_manager]") {
    auto data_manager = create_data_manager();

    test_resource_manager manager(data_manager);

    auto _ = manager.get("name1");

    REQUIRE(manager.loaded_path == "path1");

    _ = manager.get("name2");

    REQUIRE(manager.loaded_path == "path2");

    _ = manager.get("name3");

    REQUIRE(manager.loaded_path == "path3");
}
