#pragma once

#include "shared/memory/basic_allocators.h"
#include "shared/apis/logging/ilog_manager.h"

#include <cassert>
#include <memory>

namespace pbr::shared::game {
    /// Provides the skeleton of the game. The managers to be used are first configured and then added
    /// to this manager. Both the client and server will use this manager. The game loop and threads are
    /// all handled here.
    class game_manager {
    public:
        /// Constructs this manager
        /// \param log_manager The log manager to use
        game_manager(std::shared_ptr<apis::logging::ilog_manager> log_manager)
            : _log_manager(log_manager) {
            assert((this->_log_manager));
        }

        /// Destructs this manager. The game will be shutdown here
        ~game_manager() {
            if (!this->shutdown()) {
                this->_log_manager->log_message("Failed to shutdown game manager.", apis::logging::log_levels::error);
            }
        }

        game_manager(game_manager&&) = default;
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

        /// Shuts down the game
        /// \returns `true` upon success, else `false`
        [[nodiscard]]
        bool shutdown() noexcept;
    };
}
