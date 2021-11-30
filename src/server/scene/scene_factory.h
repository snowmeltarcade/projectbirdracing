#pragma once

#include "shared/scene/iscene_factory.h"
#include "shared/scene/scene_base.h"
#include "shared/apis/logging/ilog_manager.h"

#include <cassert>
#include <memory>

namespace pbr::server::scene {
    /// Creates the needed scenes for the server
    class scene_factory : public shared::scene::iscene_factory {
    public:
        scene_factory(std::shared_ptr<shared::apis::logging::ilog_manager> log_manager)
            : _log_manager(log_manager) {
            assert((this->_log_manager));
        }
        ~scene_factory() override = default;

        /// Creates a scene of the passed type
        /// \param type The type of scene to create
        /// \returns The created scene, else `nullptr` if the type is not supported
        [[nodiscard]]
        std::shared_ptr<shared::scene::scene_base> create_scene(shared::scene::scene_types type) noexcept override;

    private:
        /// The log manager to use
        std::shared_ptr<shared::apis::logging::ilog_manager> _log_manager;
    };
}