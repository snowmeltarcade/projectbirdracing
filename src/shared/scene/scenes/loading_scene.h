#pragma once

#include "shared/scene/scene_base.h"

namespace pbr::shared::scene::scenes {
    class loading_scene : public scene_base {
    public:
        loading_scene() = default;
        ~loading_scene() override = default;

        /// Loads the scene
        /// \returns `true` upon success, else `false`
        [[nodiscard]]
        bool load() noexcept override;

        /// Runs the scene
        /// \returns `true` upon success, else `false`
        [[nodiscard]]
        bool run() noexcept override;
    };
}
