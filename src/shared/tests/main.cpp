#define CATCH_CONFIG_RUNNER
#include "catch2/catch.hpp"
#include "test_utils.h"

int main(int argv, char* args[])
{
    Catch::Session session;

    current_working_directory = args[0];

    return session.run(argv, args);
}
