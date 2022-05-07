#pragma once

#include "shared/memory/basic_allocators.h"
#include "shared/apis/logging/log_manager.h"
#include "shared/data/data_manager.h"
#include "iscene_manager.h"
#include "iscene_factory.h"

#include <cassert>
#include <memory>
#include <thread>
#include <atomic>

namespace pbr::shared::scene {
    /// Manages loading, destroying and running scenes
    class scene_manager : public iscene_manager {
    public:
        /// Constructs this scene manager
        /// \param scene_factory The scene factory to use
        /// \param loading_scene_type The type to use for the loading scene. This will be loaded using the passed scene factory
        /// \param data_manager The data manager
        /// \param log_manager The log manager to use
        scene_manager(std::shared_ptr<iscene_factory> scene_factory,
                      scene_types loading_scene_type,
                      std::shared_ptr<data::data_manager> data_manager,
                      std::shared_ptr<apis::logging::ilog_manager> log_manager)
            : _scene_factory(scene_factory),
                _loading_scene_type(loading_scene_type),
                _data_manager(data_manager),
                _log_manager(log_manager) {
            assert((this->_scene_factory));
            assert((this->_data_manager));
            assert((this->_log_manager));
        }
        ~scene_manager() override {
            if (this->_new_scene_loading_thread && this->_new_scene_loading_thread->joinable()) {
                this->_new_scene_loading_thread->join();
            }
        }

        /// Runs the scene manager
        /// \returns `true` upon success else `false`
        [[nodiscard]]
        bool run() noexcept override;

    private:
        /// The scene factory to use
        std::shared_ptr<iscene_factory> _scene_factory;

        /// The loading scene type
        scene_types _loading_scene_type;

        /// Are we loading new scenes?
        std::atomic_bool _are_loading_new_scenes {false};

        /// Did the loading of new scenes fail?
        std::atomic_bool _did_loading_new_scenes_fail {false};

        /// Handles loading the new scenes
        std::unique_ptr<std::thread> _new_scene_loading_thread;

        /// The data manager
        std::shared_ptr<data::data_manager> _data_manager;

        /// The log manager to use
        std::shared_ptr<apis::logging::ilog_manager> _log_manager;

        /// The loading scene
        std::shared_ptr<scene_base> _loading_scene;

        /// The loaded scenes
        /// When loading new scenes, this will be used from the loading thread
        /// Make sure that `_are_loading_new_scenes` is false before accessing this for general use
        std::vector<std::shared_ptr<scene_base>> _loaded_scenes;

        /// Sets up loading new scenes
        /// \param have_scenes_quit If a currently loaded scene has indicated it wants to quit
        /// \returns `true` upon success else `false`.
        [[nodiscard]]
        bool setup_loading_new_scenes(bool have_scenes_quit) noexcept;

        /// Queues new scene types to load. Any currently loaded scenes will be destroyed
        /// This runs on the loading thread, so do not call this from any other thread
        /// \param types The scene types to load
        /// \returns `true` upon success else `false`. All queued scenes must successfully load to count as success.
        /// Passing an empty list of types will result in failure.
        [[nodiscard]]
        bool queue_new_scenes(const std::vector<scene_types>& types) noexcept;

        /// Constructs the passed scene. If the scene has a corresponding data file, that data is loaded
        /// and is used to build the scene. If no scene file exists, no actions are taken on the passed scene.
        /// \param scene The scene to construct
        void construct_scene(const std::shared_ptr<scene_base>& scene) noexcept;
    };
}
