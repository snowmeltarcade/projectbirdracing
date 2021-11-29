#pragma once

#include "scene_base.h"

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
    };
}
