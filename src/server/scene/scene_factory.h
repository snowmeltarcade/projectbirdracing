#pragma once

#include "shared/scene/iscene_factory.h"
#include "shared/scene/scene_base.h"

namespace pbr::server::scene {
    /// Creates the needed scenes for the server
    class scene_factory : public shared::scene::iscene_factory {
    public:
        scene_factory() = default;
        ~scene_factory() override = default;

        /// Creates a scene of the passed type
        /// \param type The type of scene to create
        /// \returns The created scene, else `nullptr` if the type is not supported
        [[nodiscard]]
        std::shared_ptr<shared::scene::scene_base> create_scene(shared::scene::scene_types type) noexcept override;
    };
}