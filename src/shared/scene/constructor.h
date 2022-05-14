#pragma once

#include "shared/apis/logging/ilog_manager.h"
#include "shared/data/settings.h"
#include "scene_base.h"

#include <memory>

namespace pbr::shared::scene {
    /// Constructs a scene from settings
    class constructor final {
    public:
        constructor() = delete;
        ~constructor() = delete;

        /// Constructs the passed scene from the passed settings
        /// \param scene The scene to construct
        /// \param settings The settings to construct from
        /// \param log_manager The log manager
        static void construct(const std::shared_ptr<scene_base>& scene,
                              const std::optional<data::settings>& settings,
                              const std::shared_ptr<apis::logging::ilog_manager>& log_manager) noexcept;
    };
}
