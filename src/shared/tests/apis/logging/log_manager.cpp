#include "catch2/catch.hpp"
#include "shared/apis/logging/log_manager.h"

using namespace pbr::shared::apis::logging;

/// A test endpoint for testing
class test_endpoint : public endpoint {
public:
    test_endpoint() = default;
    ~test_endpoint() override = default;

    /// Logs a test message
    /// \param message The message to log
    void log(std::string_view) noexcept override {
    }
};

//////////
/// add_endpoint
//////////

TEST_CASE("add_endpoint - null endpoint - returns false", "[shared/apis/logging]") {
    auto result = add_endpoint(nullptr);

    REQUIRE_FALSE(result);
}

TEST_CASE("add_endpoint - valid endpoint - returns true", "[shared/apis/logging]") {
    auto endpoint = std::make_shared<test_endpoint>();

    auto result = add_endpoint(endpoint);

    REQUIRE(result);
}

TEST_CASE("add_endpoint - valid endpoint - adds endpoint", "[shared/apis/logging]") {
    auto endpoint = std::make_shared<test_endpoint>();

    auto success = add_endpoint(endpoint);
    REQUIRE(success);

    auto endpoints = get_endpoints();

    auto result = std::find_if(endpoints.begin(), endpoints.end(),
                            [endpoint](const auto& e) { return e == endpoint; });

    REQUIRE(result != endpoints.end());
}

//////////
/// get_endpoints
//////////

TEST_CASE("get_endpoints - returns added endpoints", "[shared/apis/logging]") {
    std::vector<std::shared_ptr<endpoint>> endpoints_to_add {
        std::make_shared<test_endpoint>(),
        std::make_shared<test_endpoint>(),
        std::make_shared<test_endpoint>(),
    };

    for (const auto& e : endpoints_to_add) {
        auto success = add_endpoint(e);
        REQUIRE(success);
    }

    auto endpoints = get_endpoints();

    for (const auto& added_endpoint : endpoints_to_add) {
        auto result = std::find_if(endpoints.begin(), endpoints.end(),
                                   [added_endpoint](const auto& e) { return e == added_endpoint; });

        REQUIRE(result != endpoints.end());
    }
}

//////////
/// set_log_level
//////////

TEST_CASE("set_log_level - log level - sets the log level", "[shared/apis/logging]") {
    auto expected = log_levels::error;

    set_log_level(expected);

    auto result = get_log_level();

    REQUIRE(result == expected);
}

//////////
/// get_log_level
//////////

TEST_CASE("get_log_level - gets the log level", "[shared/apis/logging]") {
    auto expected = log_levels::error;

    set_log_level(expected);

    auto result = get_log_level();

    REQUIRE(result == expected);
}

//////////
/// log_message
//////////
