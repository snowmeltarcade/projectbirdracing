#include <iostream>
#include <vector>

#include "version.h"

int main(int argv, char* args[]) {
    std::cout << "Starting server...\n" << PROJECT_NAME << '\n' << PROJECT_NAME << '\n';

    std::vector<std::string> arguments;
    for (auto i {0}; i < argv; ++i) {
        arguments.push_back(args[i]);
    }

    std::cout << "Server complete.\n";

    return 0;
}
