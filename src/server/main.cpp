#include "shared/memory/basic_allocators.h"
#include "shared/game/game_manager.h"
#include "shared/apis/datetime/datetime_manager.h"
#include "shared/apis/logging/log_manager.h"
#include "shared/apis/logging/endpoints/std_out.h"
#include "shared/apis/logging/endpoints/file.h"
#include "shared/apis/windowing/window_manager.h"
#include "shared/apis/graphics/vulkan_graphics_manager.h"
#include "shared/apis/graphics/performance_settings.h"
#include "shared/scene/scene_manager.h"
#include "scene/scene_factory.h"
#include "shared/utils/program_arguments.h"

#include <iostream>
#include <vector>

#include "version.h"

using namespace pbr::shared;

/// Creates the game manager
/// \param arguments The program arguments
/// \returns The created game manager
game::game_manager create_game_manager(const utils::program_arguments& arguments) {
    auto datetime_manager = std::make_shared<apis::datetime::datetime_manager>();

    auto game_log_manager = std::make_shared<apis::logging::log_manager>(datetime_manager);
    if (!game_log_manager->add_endpoint(std::make_shared<apis::logging::endpoints::std_out>())) {
        throw std::logic_error("Failed to add `stdout` logging endpoint.");
    }

    auto graphics_log_manager = std::make_shared<apis::logging::log_manager>(datetime_manager);
    if (!graphics_log_manager->add_endpoint(std::make_shared<apis::logging::endpoints::std_out>())) {
        throw std::logic_error("Failed to add `stdout` logging endpoint.");
    }

    if (!graphics_log_manager->add_endpoint(std::make_shared<apis::logging::endpoints::file>(
        "graphics_log.txt",
        true))) {
        throw std::logic_error("Failed to add `file` logging endpoint.");
    }

    auto window_manager = std::make_shared<apis::windowing::window_manager>(game_log_manager);

    apis::graphics::application_information app_info {
        std::string(PROJECT_NAME) + " - Server",
        "PBREngine",
        PROJECT_VERSION_MAJOR,
        PROJECT_VERSION_MINOR,
        PROJECT_VERSION_BUILD,
    };

    apis::graphics::performance_settings performance_settings;

    auto graphics_manager = std::make_shared<apis::graphics::vulkan_graphics_manager>(graphics_log_manager,
                                                                                      window_manager,
                                                                                      app_info,
                                                                                      performance_settings);

    auto scene_factory = std::make_shared<pbr::server::scene::scene_factory>(game_log_manager);

    auto scene_manager = std::make_shared<scene::scene_manager>(scene_factory,
                                                                scene::scene_types::loading,
                                                                game_log_manager);

    auto executable_path = arguments.get_executable_path().parent_path();

    game::game_manager gm(executable_path,
                          game_log_manager,
                          window_manager,
                          graphics_manager,
                          scene_manager);
    return gm;
}

/// Sets up and runs the game
/// \param arguments The program arguments
void run(const utils::program_arguments& arguments) {
    auto gm = create_game_manager(arguments);

    if (!gm.initialize()) {
        std::cout << "Failed to initialize game manager.\n";
        return;
    }

    if (!gm.run()) {
        std::cout << "Failed to run game manager.\n";
        return;
    }
}

/// The server's main entry point
/// \param argv The length of `args`
/// \param args The program arguments
/// \return Will always return `0`
int main(int argv, char* args[]) {
    std::cout << "Starting server...\nProject Name: " << PROJECT_NAME << "\nVersion: " << PROJECT_VERSION << '\n';

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

    utils::program_arguments pa(arguments);

    run(pa);

    std::cout << "Server complete.\n";

    return 0;
}
