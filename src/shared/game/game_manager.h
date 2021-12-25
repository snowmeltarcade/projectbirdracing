#pragma once

#include "shared/memory/basic_allocators.h"
#include "shared/apis/logging/ilog_manager.h"
#include "shared/apis/windowing/iwindow_manager.h"
#include "shared/apis/graphics/igraphics_manager.h"
#include "shared/apis/windowing/iapplication_window.h"
#include "shared/scene/iscene_manager.h"

#include <cassert>
#include <memory>
#include <atomic>

namespace pbr::shared::game {
    /// Provides the skeleton of the game. The managers to be used are first configured and then added
    /// to this manager. Both the client and server will use this manager. The game loop and threads are
    /// all handled here.
    class game_manager {
    public:
        /// Constructs this manager
        /// \param log_manager The log manager to use
        /// \param window_manager The window manager to use
        /// \param graphics_manager The graphics manager to use
        /// \param scene_manager The scene manager to use
        game_manager(std::shared_ptr<apis::logging::ilog_manager> log_manager,
                     std::shared_ptr<apis::windowing::iwindow_manager> window_manager,
                     std::shared_ptr<apis::graphics::igraphics_manager> graphics_manager,
                     std::shared_ptr<scene::iscene_manager> scene_manager)
            : _log_manager(log_manager),
              _window_manager(window_manager),
              _graphics_manager(graphics_manager),
              _scene_manager(scene_manager) {
            assert((this->_log_manager));
            assert((this->_window_manager));
            assert((this->_graphics_manager));
            assert((this->_scene_manager));
        }

        /// Destructs this manager. The game will be shutdown here
        ~game_manager() {
            if (!this->shutdown()) {
                this->_log_manager->log_message("Failed to shutdown game manager.",
                                                apis::logging::log_levels::error,
                                                "Game");
            }
        }

        /// Move constructor
        /// \param The other game manager
        game_manager(game_manager&& other) {
            this->_log_manager = std::move(other._log_manager);
            this->_window_manager = std::move(other._window_manager);
            this->_graphics_manager = std::move(other._graphics_manager);
            this->_has_exit_been_requested = other._has_exit_been_requested.load();
        }
        game_manager(const game_manager&) = delete;

        /// Initializes the game
        /// \returns `true` upon success, else `false`
        [[nodiscard]]
        bool initialize() noexcept;

        /// Runs the game. This is a blocking function. It will exit when the game exits
        /// \returns `true` upon success, else `false`
        [[nodiscard]]
        bool run() noexcept;

    private:
        /// The log manager
        std::shared_ptr<apis::logging::ilog_manager> _log_manager;

        /// The window manager
        std::shared_ptr<apis::windowing::iwindow_manager> _window_manager;

        /// The graphics manager
        std::shared_ptr<apis::graphics::igraphics_manager> _graphics_manager;

        /// The main render window
        std::shared_ptr<apis::windowing::iapplication_window> _application_window;

        /// The scene manager
        std::shared_ptr<scene::iscene_manager> _scene_manager;

        /// Has an exit been requested?
        std::atomic_bool _has_exit_been_requested {false};

        /// Shuts down the game
        /// \returns `true` upon success, else `false`
        [[nodiscard]]
        bool shutdown() noexcept;

        /// Requests this game manager exits
        void request_exit() noexcept {
            this->_log_manager->log_message("Requesting exit from game manager...",
                                            apis::logging::log_levels::info,
                                            "Game");

            this->_has_exit_been_requested = true;
        }
    };
}
