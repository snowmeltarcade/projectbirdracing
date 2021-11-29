#pragma once

#include "shared/memory/basic_allocators.h"
#include "scene_types.h"

#include <vector>

namespace pbr::shared::scene {
    /// Provides an interface for the scene manager. The scene manager manages loading, destroying and running scenes
    class iscene_manager {
    public:
        iscene_manager() = default;
        virtual ~iscene_manager() = default;

        /// Initializes the scene manager
        /// \returns `true` upon success else `false`
        [[nodiscard]]
        virtual bool initialize() noexcept = 0;

        /// Runs the scene manager
        /// \returns `true` upon success else `false`
        [[nodiscard]]
        virtual bool run() noexcept = 0;

        /// Queues new scene types to load. Any currently loaded scenes will be destroyed
        /// \param types The scene types to load
        /// \returns `true` upon success else `false`. All queued scenes must successfully load to count as success.
        /// An empty list will result in failure.
        [[nodiscard]]
        virtual bool queue_new_scenes(const std::vector<scene_types>& types) noexcept = 0;
    };
}
