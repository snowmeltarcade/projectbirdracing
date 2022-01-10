#include "catch2/catch.hpp"
#include "shared/apis/file/file_manager.h"
#include "shared/tests/test_utils.h"

#include <filesystem>
#include <fstream>

using namespace pbr::shared::apis::file;

std::vector<std::byte> read_file_bytes(std::filesystem::path path) {
    std::ifstream fs(path, std::ios::ate | std::ios::binary);
    REQUIRE(fs.is_open());

    std::vector<std::byte> bytes;

    auto file_size = fs.tellg();
    bytes.resize(file_size);

    fs.seekg(0);

    fs.read(reinterpret_cast<char*>(bytes.data()), file_size);
    REQUIRE(fs.gcount() == file_size);

    fs.close();

    return bytes;
}

std::string read_file_text(std::filesystem::path path) {
    std::ifstream fs(path, std::ios::ate);
    REQUIRE(fs.is_open());

    std::string text;
    fs >> text;

    fs.close();

    return text;
}

//////////
/// read_file_bytes
//////////

TEST_CASE("read_file_bytes - invalid file path - returns error", "[shared/apis/file]") {
    file_manager manager;

    auto uri = pbr::shared::utils::build_uri("file:////path/to/file.txt");
    REQUIRE(uri);

    auto result = manager.read_file_bytes(*uri);

    REQUIRE_FALSE(result);
}

TEST_CASE("read_file_bytes - file system path - returns file bytes", "[shared/apis/file]") {
    file_manager manager;

    auto file_data_path = get_test_data_file_path("test.png");

    auto expected = read_file_bytes(file_data_path);

    auto uri = pbr::shared::utils::build_uri("file:///" + file_data_path.generic_string());
    REQUIRE(uri);

    auto result = manager.read_file_bytes(*uri);
    REQUIRE(result);

    REQUIRE(expected.size() == result->size());
    for (auto i {0u}; i < expected.size(); ++i) {
        REQUIRE(expected[i] == (*result)[i]);
    }
}

//////////
/// read_file_bytes_async
//////////

TEST_CASE("read_file_bytes_async - invalid file path - returns error", "[shared/apis/file]") {
    file_manager manager;

    auto uri = pbr::shared::utils::build_uri("file:////path/to/file.txt");
    REQUIRE(uri);

    auto result_future = manager.read_file_bytes_async(*uri);
    auto result = result_future.get();

    REQUIRE_FALSE(result);
}

TEST_CASE("read_file_bytes_async - file system path - returns file bytes", "[shared/apis/file]") {
    file_manager manager;

    auto file_data_path = get_test_data_file_path("test.png");

    auto expected = read_file_bytes(file_data_path);

    auto uri = pbr::shared::utils::build_uri("file:///" + file_data_path.generic_string());
    REQUIRE(uri);

    auto result_future = manager.read_file_bytes_async(*uri);
    auto result = result_future.get();
    REQUIRE(result);

    REQUIRE(expected.size() == result->size());
    for (auto i {0u}; i < expected.size(); ++i) {
        REQUIRE(expected[i] == (*result)[i]);
    }
}

//////////
/// read_file_text
//////////

TEST_CASE("read_file_text - invalid file path - returns error", "[shared/apis/file]") {
    file_manager manager;

    auto uri = pbr::shared::utils::build_uri("file:////path/to/file.txt");
    REQUIRE(uri);

    auto result = manager.read_file_text(*uri);

    REQUIRE_FALSE(result);
}

TEST_CASE("read_file_text - file system path - returns file text", "[shared/apis/file]") {
    file_manager manager;

    auto file_data_path = get_test_data_file_path("text.txt");

    auto expected = read_file_text(file_data_path);

    auto uri = pbr::shared::utils::build_uri("file:///" + file_data_path.generic_string());
    REQUIRE(uri);

    auto result = manager.read_file_text(*uri);
    REQUIRE(result);

    REQUIRE(expected == *result);
}

//////////
/// read_file_text_async
//////////

TEST_CASE("read_file_text_async - invalid file path - returns error", "[shared/apis/file]") {
    file_manager manager;

    auto uri = pbr::shared::utils::build_uri("file:////path/to/file.txt");
    REQUIRE(uri);

    auto result_future = manager.read_file_text_async(*uri);
    auto result = result_future.get();

    REQUIRE_FALSE(result);
}

TEST_CASE("read_file_text_async - file system path - returns file text", "[shared/apis/file]") {
    file_manager manager;

    auto file_data_path = get_test_data_file_path("text.txt");

    auto expected = read_file_text(file_data_path);

    auto uri = pbr::shared::utils::build_uri("file:///" + file_data_path.generic_string());
    REQUIRE(uri);

    auto result_future = manager.read_file_text_async(*uri);
    auto result = result_future.get();
    REQUIRE(result);

    REQUIRE(expected == *result);
}
