#include <iostream>
#include <vector>

#include "version.h"

int main(int argv, char* args[]) {
    std::cout << "Starting client...\nProject Name: " << PROJECT_NAME << "\nVersion: " << PROJECT_VERSION << '\n';

    auto mode { "UNKNOWN" };
    // doesn't seem clang supports `#elifdef` yet: https://reviews.llvm.org/D101192
#ifdef DEBUG
    mode = "DEBUG";
#endif
#ifdef RELEASE_WITH_DEBUG_INFO
    mode = "RELEASE WITH DEBUG INFO";
#endif
#ifdef RELEASE
    mode = "RELEASE";
#endif
    std::cout << "Running in " << mode << '\n';

    std::vector<std::string> arguments;
    for (auto i {0}; i < argv; ++i) {
        arguments.push_back(args[i]);
    }

    std::cout << "Client complete.\n";

    return 0;
}
