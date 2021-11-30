#pragma once

#include "scene_types.h"
#include "shared/apis/logging/ilog_manager.h"

#include <cassert>
#include <memory>

namespace pbr::shared::scene {
    /// Provides the base functionality of a scene. This manages the ECS, UI, physics and world
    class scene_base {
    public:
        /// Constructs the base scene
        /// \param log_manager The log manager to use
        scene_base(std::shared_ptr<apis::logging::ilog_manager> log_manager)
            : _log_manager(log_manager) {
            assert((this->_log_manager));
        }
        virtual ~scene_base() = default;

        /// Loads the scene
        /// \returns `true` upon success, else `false`
        [[nodiscard]]
        virtual bool load() noexcept = 0;

        /// Runs the scene
        /// \returns `true` upon success, else `false`
        [[nodiscard]]
        virtual bool run() noexcept = 0;

    protected:
        /// The log manager to use
        std::shared_ptr<apis::logging::ilog_manager> _log_manager;
    };
}
