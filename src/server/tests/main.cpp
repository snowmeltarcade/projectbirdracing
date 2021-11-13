#define CATCH_CONFIG_RUNNER
#include "catch2/catch.hpp"

int main(int argv, char* args[])
{
    Catch::Session session;

    return session.run(argv, args);
}

TEST_CASE("test", "[server/test]") {
    REQUIRE(true);
}
