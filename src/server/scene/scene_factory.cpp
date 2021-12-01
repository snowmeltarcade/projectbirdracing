#include "scene_factory.h"
#include "shared/scene/scenes/loading_scene.h"
#include "shared/scene/scenes/splash_screen_scene.h"
#include "shared/scene/scenes/world_generation_scene.h"

#include <cassert>

namespace pbr::server::scene {
    std::shared_ptr<shared::scene::scene_base> scene_factory::create_scene(shared::scene::scene_types type) noexcept {
        // use an if here rather than a switch to avoid any clang tidy errors...
        if (type == shared::scene::scene_types::loading) {
            return std::make_shared<shared::scene::scenes::loading_scene>(this->_log_manager);
        } else if (type == shared::scene::scene_types::splash_screen) {
            return std::make_shared<shared::scene::scenes::splash_screen_scene>(this->_log_manager);
        } else if (type == shared::scene::scene_types::world_generation) {
            return std::make_shared<shared::scene::scenes::world_generation_scene>(this->_log_manager);
        }

        return {};
    }

    std::vector<shared::scene::scene_types> scene_factory::get_next_scenes(const std::vector<std::shared_ptr<shared::scene::scene_base>>& current_scenes) noexcept {
        assert((!current_scenes.empty()));

        auto current_type = current_scenes[0]->get_scene_type();

        if (current_type == shared::scene::scene_types::splash_screen) {
            return { shared::scene::scene_types::world_generation };
        }

        this->_log_manager->log_message("Invalid scene state machine state.", shared::apis::logging::log_levels::error);
        return { shared::scene::scene_types::loading };
    }
}
