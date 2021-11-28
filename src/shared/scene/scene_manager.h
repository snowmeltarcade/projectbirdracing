#pragma once

#include "shared/memory/basic_allocators.h"
#include "shared/apis/logging/log_manager.h"
#include "iscene_manager.h"

#include <cassert>
#include <memory>

namespace pbr::shared::scene {
    /// Manages loading, destroying and running scenes
    class scene_manager : public iscene_manager {
    public:
        /// Constructs this scene manager
        /// \param log_manager The log manager to use
        scene_manager(std::shared_ptr<apis::logging::ilog_manager> log_manager)
            : _log_manager(log_manager) {
            assert((this->_log_manager));
        }
        ~scene_manager() override = default;

        /// Runs the scene manager
        /// \returns `true` upon success else `false`
        [[nodiscard]]
        bool run() noexcept override;

    private:
        /// The log manager to use
        std::shared_ptr<apis::logging::ilog_manager> _log_manager;
    };
}
