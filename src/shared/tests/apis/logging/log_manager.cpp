#include "catch2/catch.hpp"
#include "shared/apis/logging/log_manager.h"
#include "shared/apis/datetime/datetime_manager.h"
#include "shared/utils/strings.h"

#include <thread>
#include <memory>

using namespace pbr::shared::apis::logging;
using namespace pbr::shared::utils;

/// A test endpoint for testing
class test_endpoint : public endpoint {
public:
    test_endpoint() {
        // reset the call count for each test
        _log_call_count = 0u;
    }
    ~test_endpoint() override = default;

    /// The number of times `log()` is called
    static uint32_t _log_call_count;

    /// The message that was logged
    std::string _logged_message;

    /// Logs a test message
    /// \param message The message to log
    void log(std::string_view message) noexcept override {
        ++test_endpoint::_log_call_count;

        this->_logged_message = message;
    }
};

uint32_t test_endpoint::_log_call_count {0u};

//////////
/// add_endpoint
//////////

TEST_CASE("add_endpoint - null endpoint - returns false", "[shared/apis/logging]") {
    auto datetime_manager = std::make_shared<pbr::shared::apis::datetime::datetime_manager>();
    log_manager l(datetime_manager);
    
    auto result = l.add_endpoint(nullptr);

    REQUIRE_FALSE(result);
}

TEST_CASE("add_endpoint - valid endpoint - returns true", "[shared/apis/logging]") {
    auto datetime_manager = std::make_shared<pbr::shared::apis::datetime::datetime_manager>();
    log_manager l(datetime_manager);

    auto endpoint = std::make_shared<test_endpoint>();

    auto result = l.add_endpoint(endpoint);

    REQUIRE(result);
}

TEST_CASE("add_endpoint - valid endpoint - adds endpoint", "[shared/apis/logging]") {
    auto datetime_manager = std::make_shared<pbr::shared::apis::datetime::datetime_manager>();
    log_manager l(datetime_manager);

    auto endpoint = std::make_shared<test_endpoint>();

    auto success = l.add_endpoint(endpoint);
    REQUIRE(success);

    auto endpoints = l.get_endpoints();

    auto result = std::find_if(endpoints.begin(), endpoints.end(),
                            [endpoint](const auto& e) { return e == endpoint; });

    REQUIRE(result != endpoints.end());
}

//////////
/// get_endpoints
//////////

TEST_CASE("get_endpoints - returns added endpoints", "[shared/apis/logging]") {
    auto datetime_manager = std::make_shared<pbr::shared::apis::datetime::datetime_manager>();
    log_manager l(datetime_manager);

    std::vector<std::shared_ptr<endpoint>> endpoints_to_add {
        std::make_shared<test_endpoint>(),
        std::make_shared<test_endpoint>(),
        std::make_shared<test_endpoint>(),
    };

    for (const auto& e : endpoints_to_add) {
        auto success = l.add_endpoint(e);
        REQUIRE(success);
    }

    auto endpoints = l.get_endpoints();

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
    auto datetime_manager = std::make_shared<pbr::shared::apis::datetime::datetime_manager>();
    log_manager l(datetime_manager);

    auto expected = log_levels::error;

    l.set_log_level(expected);

    auto result = l.get_log_level();

    REQUIRE(result == expected);
}

//////////
/// get_log_level
//////////

TEST_CASE("get_log_level - gets the log level", "[shared/apis/logging]") {
    auto datetime_manager = std::make_shared<pbr::shared::apis::datetime::datetime_manager>();
    log_manager l(datetime_manager);

    auto expected = log_levels::error;

    l.set_log_level(expected);

    auto result = l.get_log_level();

    REQUIRE(result == expected);
}

//////////
/// log_message
//////////

TEST_CASE("log_message - valid message and level - logs the message to all endpoints", "[shared/apis/logging]") {
    auto datetime_manager = std::make_shared<pbr::shared::apis::datetime::datetime_manager>();
    log_manager l(datetime_manager);

    std::vector<std::shared_ptr<endpoint>> endpoints {
        std::make_shared<test_endpoint>(),
        std::make_shared<test_endpoint>(),
        std::make_shared<test_endpoint>(),
    };

    auto message { "message" };
    auto level { log_levels::info };

    for (const auto& e : endpoints) {
        REQUIRE(l.add_endpoint(e));
    }

    l.log_message(message, level);

    auto result = test_endpoint::_log_call_count;
    auto expected = endpoints.size();

    REQUIRE(result == expected);
}

TEST_CASE("log_message - high log level - ignores messages from lower levels", "[shared/apis/logging]") {
    auto datetime_manager = std::make_shared<pbr::shared::apis::datetime::datetime_manager>();
    log_manager l(datetime_manager);

    std::vector<std::shared_ptr<endpoint>> endpoints {
        std::make_shared<test_endpoint>(),
        std::make_shared<test_endpoint>(),
        std::make_shared<test_endpoint>(),
    };

    auto message { "message" };
    auto level { log_levels::info };
    auto high_log_level { log_levels::error };

    for (const auto& e : endpoints) {
        REQUIRE(l.add_endpoint(e));
    }

    l.set_log_level(high_log_level);

    l.log_message(message, level);

    auto result = test_endpoint::_log_call_count;
    auto expected = 0u;

    REQUIRE(result == expected);
}

TEST_CASE("log_message - single thread - passes log message to all endpoints", "[shared/apis/logging]") {
    auto datetime_manager = std::make_shared<pbr::shared::apis::datetime::datetime_manager>();
    log_manager l(datetime_manager);

    std::vector<std::shared_ptr<test_endpoint>> endpoints {
        std::make_shared<test_endpoint>(),
        std::make_shared<test_endpoint>(),
        std::make_shared<test_endpoint>(),
    };

    auto message { "message" };
    auto level { log_levels::info };

    for (const auto& e : endpoints) {
        REQUIRE(l.add_endpoint(e));
    }

    l.log_message(message, level);

    for (const auto& e : endpoints) {
        auto result = ends_with(e->_logged_message, message);
        REQUIRE(result);
    }
}

TEST_CASE("log_message - multiple threads - passes log message to all endpoints", "[shared/apis/logging]") {
    auto datetime_manager = std::make_shared<pbr::shared::apis::datetime::datetime_manager>();
    log_manager l(datetime_manager);

    auto number_of_threads { 1000u };
    std::vector<std::shared_ptr<test_endpoint>> endpoints;
    auto message { "message" };
    auto level { log_levels::info };

    for (auto i { 0u }; i < number_of_threads; ++i) {
        auto e = std::make_shared<test_endpoint>();
        endpoints.push_back(e);
        REQUIRE(l.add_endpoint(e));
    }

    std::vector<std::thread> threads;

    // log the message to all endpoints `number_of_threads` of times
    for (auto i { 0u }; i < number_of_threads; ++i) {
        threads.emplace_back([&l, message, level](){
            l.log_message(message, level);
        });
    }

    for (auto& t : threads) {
        t.join();
    }

    for (const auto& e : endpoints) {
        auto result = ends_with(e->_logged_message, message);
        REQUIRE(result);
    }
}
