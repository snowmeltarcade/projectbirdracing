#include "catch2/catch.hpp"
#include "test_utils.h"
#include "shared/data/settings.h"

using namespace pbr::shared;
using namespace pbr::shared::data;

//////////
/// add
//////////

TEST_CASE("add - valid key and value - adds setting", "[shared/data]") {
    std::string key1 = "my key1";
    std::string key2 = "my key2";
    std::string key3 = "my key3";
    std::string key4 = "";

    std::string expected1 = "my value";

    auto value2 = 42;
    std::string expected2 = "42";

    auto value3 = true;
    std::string expected3 = "true";

    settings expected4;
    expected4.add("key", "value");

    settings settings;
    settings.add(key1, expected1);
    settings.add(key2, value2);
    settings.add(key3, value3);
    settings.add(key4, expected4);

    auto result = settings.get(key1);
    REQUIRE(result == expected1);

    result = settings.get(key2);
    REQUIRE(result == expected2);

    result = settings.get(key3);
    REQUIRE(result == expected3);

    auto result_settings = settings.get_as_settings(key4);
    REQUIRE(result_settings == expected4);
}

//////////
/// add_to_array
//////////

TEST_CASE("add_to_array - valid key and value - adds setting to array", "[shared/data]") {
    auto key1 = "key1";
    auto expected1 = "expected1";

    settings settings1;
    settings1.add(key1, expected1);

    auto key2 = "key2";
    auto expected2 = "expected2";

    settings settings2;
    settings2.add(key2, expected2);

    auto key = "key";

    settings settings;
    settings.add_to_array(key, settings1);
    settings.add_to_array(key, settings2);

    auto result = settings.get_as_settings_array(key);
    REQUIRE(result);
    REQUIRE(result->size() == 2);
    REQUIRE((*result)[0].get(key1) == expected1);
    REQUIRE((*result)[1].get(key2) == expected2);
}

//////////
/// get
//////////

TEST_CASE("get - unknown key - returns empty", "[shared/data]") {
    settings settings;

    auto result = settings.get("unknown");

    REQUIRE_FALSE(result);
}

TEST_CASE("get - known key - returns value", "[shared/data]") {
    settings settings;

    std::string key = "key";
    std::string expected = "value";

    settings.add(key, expected);

    auto result = settings.get(key);

    REQUIRE(result == expected);
}

TEST_CASE("get - existing key - overwrites value", "[shared/data]") {
    settings settings;

    std::string key = "key";
    std::string expected = "value2";

    settings.add(key, "value");
    settings.add(key, expected);

    auto result = settings.get(key);

    REQUIRE(result == expected);
}

//////////
/// get_as_settings
//////////

TEST_CASE("get_as_settings - unknown key - returns empty", "[shared/data]") {
    settings settings;

    auto result = settings.get_as_settings("unknown");

    REQUIRE_FALSE(result);
}

TEST_CASE("get_as_settings - known key - returns value", "[shared/data]") {
    settings expected;
    expected.add("key", "value");

    // add a copy of this settings object to itself
    expected.add("key2", expected);

    std::string key = "key";

    settings settings;
    settings.add(key, expected);

    auto result = settings.get_as_settings(key);

    REQUIRE(result == expected);
}

TEST_CASE("get_as_settings - existing key - overwrites value", "[shared/data]") {
    settings old_settings;
    old_settings.add("key", "value");

    settings expected;
    expected.add("key2", "value2");

    std::string key = "key";

    settings settings;
    settings.add(key, old_settings);
    settings.add(key, expected);

    auto result = settings.get_as_settings(key);

    REQUIRE(result == expected);
}

//////////
/// get_as_settings_array
//////////

TEST_CASE("get_as_settings_array - unknown key - returns empty", "[shared/data]") {
    settings settings;

    auto result = settings.get_as_settings_array("unknown");

    REQUIRE_FALSE(result);
}

TEST_CASE("get_as_settings_array - known key - returns value", "[shared/data]") {
    settings expected;
    auto key = "key";
    expected.add(key, "value");

    settings settings;
    auto array_key = "key2";
    settings.add_to_array(array_key, expected);

    auto result = settings.get_as_settings_array(array_key);
    REQUIRE(result);
    REQUIRE((*result)[0].get(key) == expected.get(key));
}

//////////
/// get_as_int
//////////

TEST_CASE("get_as_int - unknown key - returns empty", "[shared/data]") {
    settings settings;

    auto result = settings.get_as_int("unknown");

    REQUIRE_FALSE(result);
}

TEST_CASE("get_as_int - invalid value - returns empty", "[shared/data]") {
    settings settings;

    auto key = "key";
    settings.add(key, "invalid");

    auto result = settings.get_as_int(key);

    REQUIRE_FALSE(result);
}

TEST_CASE("get_as_int - known key - returns value", "[shared/data]") {
    settings settings;

    std::string key = "key";
    auto expected = 1234;

    settings.add(key, expected);

    auto result = settings.get_as_int(key);

    REQUIRE(result);
    REQUIRE(*result == expected);
}

TEST_CASE("get_as_int - existing key - overwrites value", "[shared/data]") {
    settings settings;

    std::string key = "key";
    auto expected = 1234;

    settings.add(key, 9999);
    settings.add(key, expected);

    auto result = settings.get_as_int(key);

    REQUIRE(result);
    REQUIRE(*result == expected);
}

//////////
/// get_as_uint32_t
//////////

TEST_CASE("get_as_uint32_t - unknown key - returns empty", "[shared/data]") {
    settings settings;

    auto result = settings.get_as_uint32_t("unknown");

    REQUIRE_FALSE(result);
}

TEST_CASE("get_as_uint32_t - invalid value - returns empty", "[shared/data]") {
    settings settings;

    auto key = "key";
    settings.add(key, "invalid");

    auto result = settings.get_as_uint32_t(key);

    REQUIRE_FALSE(result);
}

TEST_CASE("get_as_uint32_t - known key - returns value", "[shared/data]") {
    settings settings;

    std::string key = "key";
    auto expected = 1234u;

    settings.add(key, expected);

    auto result = settings.get_as_uint32_t(key);

    REQUIRE(result);
    REQUIRE(*result == expected);
}

TEST_CASE("get_as_uint32_t - existing key - overwrites value", "[shared/data]") {
    settings settings;

    std::string key = "key";
    auto expected = 1234u;

    settings.add(key, 9999u);
    settings.add(key, expected);

    auto result = settings.get_as_uint32_t(key);

    REQUIRE(result);
    REQUIRE(*result == expected);
}

//////////
/// get_as_float
//////////

TEST_CASE("get_as_float - unknown key - returns empty", "[shared/data]") {
    settings settings;

    auto result = settings.get_as_float("unknown");

    REQUIRE_FALSE(result);
}

TEST_CASE("get_as_float - invalid value - returns empty", "[shared/data]") {
    settings settings;

    auto key = "key";
    settings.add(key, "invalid");

    auto result = settings.get_as_float(key);

    REQUIRE_FALSE(result);
}

TEST_CASE("get_as_float - known key - returns value", "[shared/data]") {
    settings settings;

    std::string key = "key";
    auto expected = 123.456f;

    settings.add(key, expected);

    auto result = settings.get_as_float(key);

    REQUIRE(result);
    REQUIRE(*result == expected);
}

TEST_CASE("get_as_float - existing key - overwrites value", "[shared/data]") {
    settings settings;

    std::string key = "key";
    auto expected = 123.456f;

    settings.add(key, 999.999f);
    settings.add(key, expected);

    auto result = settings.get_as_float(key);

    REQUIRE(result);
    REQUIRE(*result == expected);
}

//////////
/// get_as_bool
//////////

TEST_CASE("get_as_bool - unknown key - returns empty", "[shared/data]") {
    settings settings;

    auto result = settings.get_as_bool("unknown");

    REQUIRE_FALSE(result);
}

TEST_CASE("get_as_bool - invalid value - returns empty", "[shared/data]") {
    settings settings;

    auto key = "key";
    settings.add(key, "invalid");

    auto result = settings.get_as_bool(key);

    REQUIRE_FALSE(result);
}

TEST_CASE("get_as_bool - known key - returns value", "[shared/data]") {
    settings settings;

    std::string key = "key";
    auto expected = true;

    settings.add(key, expected);

    auto result = settings.get_as_bool(key);

    REQUIRE(result);
    REQUIRE(*result == expected);
}

TEST_CASE("get_as_bool - existing key - overwrites value", "[shared/data]") {
    settings settings;

    std::string key = "key";
    auto expected = true;

    settings.add(key, false);
    settings.add(key, expected);

    auto result = settings.get_as_bool(key);

    REQUIRE(result);
    REQUIRE(*result == expected);
}
