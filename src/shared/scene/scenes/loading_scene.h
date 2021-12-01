#pragma once

#include "shared/scene/scene_base.h"

namespace pbr::shared::scene::scenes {
    class loading_scene : public scene_base {
    public:
        /// Constructs the loading scene
        /// \param log_manager The log manager to use
        loading_scene(std::shared_ptr<apis::logging::ilog_manager> log_manager)
            : scene_base(log_manager) {
        }
        ~loading_scene() override = default;

        /// Loads the scene
        /// \returns `true` upon success, else `false`
        [[nodiscard]]
        bool load() noexcept override;

        /// Runs the scene
        /// \returns `true` upon success, else `false`
        [[nodiscard]]
        bool run() noexcept override;

        /// This is ignored as this is the scene used when loading other scenes
        /// \returns `true` if this scene should quit, else `false`
        [[nodiscard]]
        bool should_quit() const noexcept override {
            return false;
        }
    };
}
