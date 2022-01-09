#include "catch2/catch.hpp"
#include "shared/apis/file/file_manager.h"

using namespace pbr::shared::apis::file;

//////////
/// read_file_data
//////////

TEST_CASE("read_file_data - invalid file path - returns error", "[shared/apis/file]") {
    file_manager manager;

    auto result = manager.read_file_data();

    REQUIRE_FALSE(result);
}
