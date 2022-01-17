#include <string>
#include <array>

#include "catch2/catch.hpp"
#include "shared/utils/program_arguments.h"

using namespace std::literals;
using namespace pbr::shared::utils;

/*********************************************
 * constructor
 ********************************************/

TEST_CASE("constructor - empty arguments - throws exception", "[shared/utils/program_arguments]") {
    std::vector<std::string> arguments;

    try {
        program_arguments pa(arguments);
    } catch (...) {
        SUCCEED("");
        return;
    }

    FAIL("");
}

//TEST_CASE("constructor - invalid arguments - throws exception", "[shared/utils/program_arguments]") {
//    std::vector<std::string> arguments {
//        "file/path/executable.exe",
//        "invalid",
//    };
//
//    try {
//        program_arguments pa(arguments);
//    } catch (...) {
//        SUCCEED();
//        return;
//    }
//
//    FAIL();
//}

/*********************************************
 * get_executable_path
 ********************************************/

TEST_CASE("get_executable_path - returns the executable's path", "[shared/utils/program_arguments]") {
    std::vector<std::string> arguments {
        "file/path/executable.exe",
//        "arg1=value1",
//        "arg2=value2",
//        "arg3=value3",
    };

    program_arguments pa(arguments);

    auto result = pa.get_executable_path();

    REQUIRE(result == arguments[0]);
}
