#pragma once

#include "scene_types.h"

namespace pbr::shared::scene {
    /// Provides the base functionality of a scene. This manages the ECS, UI, physics and world
    class scene_base {
    public:
        scene_base() = default;
        virtual ~scene_base() = default;
    };
}
