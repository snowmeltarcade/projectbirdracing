#pragma once

#include "scene_base.h"
#include "scene_types.h"

#include <vector>
#include <memory>

namespace pbr::shared::scene {
    /// Provides an interface to a scene factory. A scene factory allows the creation of a scene by an id
    class iscene_factory {
    public:
        iscene_factory() = default;
        virtual ~iscene_factory() = default;

        /// Creates a scene of the passed type
        /// \param type The type of scene to create
        /// \returns The created scene, else `nullptr` if the type is not supported
        [[nodiscard]]
        virtual std::shared_ptr<scene_base> create_scene(scene_types type) noexcept = 0;

        /// Get the next scenes to load based on the passed scenes
        /// \param current_scenes The currently running scenes
        /// \returns The next scene types to load (can contain duplicate scene types)
        [[nodiscard]]
        virtual std::vector<scene_types> get_next_scenes(const std::vector<std::shared_ptr<scene_base>>& current_scenes) noexcept = 0;
    };
}
