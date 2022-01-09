#include "catch2/catch.hpp"
#include "shared/apis/file/file_manager.h"

using namespace pbr::shared::apis::file;

//////////
/// read_file_data
//////////

TEST_CASE("read_file_data - invalid file path - returns error", "[shared/apis/file]") {
    file_manager manager;

    auto uri = pbr::shared::utils::build_uri("file:///path/to/file.txt");
    REQUIRE(uri);

    auto result = manager.read_file_data(*uri);

    REQUIRE_FALSE(result);
}
