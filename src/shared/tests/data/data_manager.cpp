#include "catch2/catch.hpp"
#include "test_utils.h"
#include "shared/data/data_manager.h"
#include "shared/apis/datetime/datetime_manager.h"
#include "shared/apis/logging/log_manager.h"
#include "shared/apis/file/file_manager.h"

using namespace pbr::shared;
using namespace pbr::shared::data;

static data_manager create_data_manager() {
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

TEST_CASE("read_settings - valid settings file - returns valid settings", "[shared/data]") {
    auto dm = create_data_manager();

    auto path = "settings";

    auto result = dm.read_settings(path);
    REQUIRE(result);

    REQUIRE(result->get("string1") == "value1");
    REQUIRE(result->get("string2") == "value2");
    REQUIRE(result->get_as_int("int1") == -123);
    REQUIRE(result->get_as_int("int2") == 456);
    REQUIRE(result->get_as_uint32_t("int3") == 1234567890);
    REQUIRE(result->get_as_float("float1") == 123.456f);
    REQUIRE(result->get_as_float("float2") == -789.012f);
    REQUIRE(result->get_as_bool("bool1") == true);
    REQUIRE(result->get_as_bool("bool2") == false);
    REQUIRE(result->get("null1") == "");

    auto settings1 = result->get_as_settings("settings1");
    REQUIRE(settings1);
    REQUIRE(settings1->get("string3") == "value3");

    auto settings2 = result->get_as_settings("settings2");
    REQUIRE(settings2);
    REQUIRE(settings2->get_as_float("float3") == 987.654f);
    REQUIRE(settings2->get_as_float("float4") == 123.123f);

    auto array1 = result->get_as_settings_array("array1");
    REQUIRE(array1);
    REQUIRE(array1->size() == 2);
    REQUIRE((*array1)[0].get("string1") == "value1");
    REQUIRE((*array1)[0].get("string2") == "value2");
    REQUIRE((*array1)[1].get("string3") == "value3");
    REQUIRE((*array1)[1].get("string4") == "value4");
}

//////////
/// read_shader_code
//////////

TEST_CASE("read_shader_code - invalid path - returns empty", "[shared/data]") {
    auto dm = create_data_manager();

    auto path = "invalid";

    auto result = dm.read_shader_code(path);

    REQUIRE_FALSE(result);
}

TEST_CASE("read_shader_code - valid path - returns shader code", "[shared/data]") {
    auto dm = create_data_manager();

    auto path = "shader_vertex";

    auto result = dm.read_shader_code(path);
    REQUIRE(result);

    auto expected = "#version 330 core\n"
                    "\n"
                    "layout (location = 0) in vec2 aPos;\n"
                    "\n"
                    "void main()\n"
                    "{\n"
                    "    gl_Position = vec4(aPos, 0, 1);\n"
                    "}\n";

    REQUIRE(result->code == expected);
}

TEST_CASE("read_shader_code - valid path - returns correct shader types", "[shared/data]") {
    auto dm = create_data_manager();

    std::vector<std::pair<std::string, apis::graphics::shader_types>> test_data {
        { "shader_vertex", apis::graphics::shader_types::vertex, },
        { "shader_fragment", apis::graphics::shader_types::fragment, },
        { "shader_unknown", apis::graphics::shader_types::fragment, },
    };

    for (const auto& [path, expected] : test_data) {
        auto result = dm.read_shader_code(path, expected);
        REQUIRE(result);

        REQUIRE(result->type == expected);
    }
}
