#include <string>
#include <array>

#include "catch2/catch.hpp"
#include "shared/utils/uri.h"

using namespace std::literals;
using namespace pbr::shared::utils;

/*********************************************
 * build_uri
 ********************************************/

TEST_CASE("build_uri - empty string - returns error", "[shared/utils/uri]") {
    auto s = "";

    auto result = build_uri(s);

    REQUIRE_FALSE(result);
}

TEST_CASE("build_uri - invalid uri - returns error", "[shared/utils/uri]") {
    auto s = "invalid uri...";

    auto result = build_uri(s);

    REQUIRE_FALSE(result);
}

TEST_CASE("build_uri - valid uri - returns correct results", "[shared/utils/uri]") {
    // we're only testing for what we need right now...
    std::vector<std::pair<std::string, uri>> uris {
        { "file:////path/to/my/file.txt", { "file", "/path/to/my/file.txt", }, },
        { "file:///C:/path/to/my/file.txt", { "file", "C:/path/to/my/file.txt", }, },
        { "file:///C:\\path\\to\\my\\file.txt", { "file", "C:\\path\\to\\my\\file.txt", }, },
    };

    for (auto& [uri_string, expected] : uris) {
        auto result = build_uri(uri_string);
        REQUIRE(result);

        REQUIRE(*result == expected);
    }
}
