#include "scene_factory.h"
#include "shared/scene/scenes/loading_scene.h"

namespace pbr::server::scene {
    std::shared_ptr<shared::scene::scene_base> scene_factory::create_scene(shared::scene::scene_types type) noexcept {
        // use an if here rather than a switch to avoid any clang tidy errors...
        if (type == shared::scene::scene_types::loading) {
            return std::make_shared<shared::scene::scenes::loading_scene>();
        }

        return {};
    }
}
