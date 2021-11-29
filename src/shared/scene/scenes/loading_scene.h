#pragma once

#include "shared/scene/scene_base.h"

namespace pbr::shared::scene::scenes {
    class loading_scene : public scene_base {
    public:
        loading_scene() = default;
        ~loading_scene() override = default;
    };
}
