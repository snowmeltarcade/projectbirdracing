#pragma once

#include "shared/scene/scene_base.h"

namespace pbr::shared::scene::scenes {
    class world_generation_scene : public scene_base {
    public:
        /// Constructs the world generation scene
        /// \param log_manager The log manager to use
        world_generation_scene(std::shared_ptr<apis::logging::ilog_manager> log_manager)
            : scene_base(log_manager) {
        }
        ~world_generation_scene() override = default;

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
