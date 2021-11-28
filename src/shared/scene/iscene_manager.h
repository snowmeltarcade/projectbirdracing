#pragma once

namespace pbr::shared::scene {
    /// Provides an interface for the scene manager. The scene manager manages loading, destroying and running scenes
    class iscene_manager {
    public:
        iscene_manager() = default;
        virtual ~iscene_manager() = default;

        /// Runs the scene manager
        /// \returns `true` upon success else `false`
        [[nodiscard]]
        virtual bool run() noexcept = 0;
    };
}
