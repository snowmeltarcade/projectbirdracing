#pragma once

#include "shared/memory/basic_allocators.h"
#include "shared/apis/logging/log_manager.h"
#include "iscene_manager.h"
#include "iscene_factory.h"

#include <cassert>
#include <memory>

namespace pbr::shared::scene {
    /// Manages loading, destroying and running scenes
    class scene_manager : public iscene_manager {
    public:
        /// Constructs this scene manager
        /// \param scene_factory The scene factory to use
        /// \param loading_scene_type The type to use as the loading scene. This will be loaded using the passed scene factory
        /// \param log_manager The log manager to use
        scene_manager(std::shared_ptr<iscene_factory> scene_factory,
                      scene_types loading_scene_type,
                      std::shared_ptr<apis::logging::ilog_manager> log_manager)
            : _scene_factory(scene_factory),
                _loading_scene_type(loading_scene_type),
                _log_manager(log_manager) {
            assert((this->_scene_factory));
            assert((this->_log_manager));
        }
        ~scene_manager() override = default;

        /// Initializes the scene manager. This will also load the `loading` scene
        /// \returns `true` upon success else `false`
        [[nodiscard]]
        bool initialize() noexcept override;

        /// Runs the scene manager
        /// \returns `true` upon success else `false`
        [[nodiscard]]
        bool run() noexcept override;

        /// Queues new scene types to load. Any currently loaded scenes will be destroyed
        /// \param types The scene types to load
        /// \returns `true` upon success else `false`. All queued scenes must successfully load to count as success.
        /// An empty list will result in failure.
        [[nodiscard]]
        bool queue_new_scenes(const std::vector<scene_types>& types) noexcept override;

    private:
        /// The scene factory to use
        std::shared_ptr<iscene_factory> _scene_factory;

        /// The loading scene type
        scene_types _loading_scene_type;

        /// The log manager to use
        std::shared_ptr<apis::logging::ilog_manager> _log_manager;

        /// The loaded scenes
        std::vector<std::shared_ptr<scene_base>> _loaded_scenes;
    };
}
