#pragma once

#include "shared/memory/basic_allocators.h"
#include "shared/apis/logging/ilog_manager.h"
#include "shared/apis/windowing/iwindow_manager.h"

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
        game_manager(std::shared_ptr<apis::logging::ilog_manager> log_manager,
                     std::shared_ptr<apis::windowing::iwindow_manager> window_manager)
            : _log_manager(log_manager),
              _window_manager(window_manager) {
            assert((this->_log_manager));
            assert((this->_window_manager));
        }

        /// Destructs this manager. The game will be shutdown here
        ~game_manager() {
            if (!this->shutdown()) {
                this->_log_manager->log_message("Failed to shutdown game manager.", apis::logging::log_levels::error);
            }
        }

        /// Move constructor
        /// \param The other game manager
        game_manager(game_manager&& other) {
            this->_log_manager = std::move(other._log_manager);
            this->_window_manager = std::move(other._window_manager);
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

        /// Has an exit been requested?
        std::atomic_bool _has_exit_been_requested {false};

        /// Shuts down the game
        /// \returns `true` upon success, else `false`
        [[nodiscard]]
        bool shutdown() noexcept;

        /// Requests this game manager exits
        void request_exit() noexcept {
            this->_has_exit_been_requested = true;
        }
    };
}
