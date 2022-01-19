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
#include <filesystem>
#include <thread>
#include <semaphore>

namespace pbr::shared::game {
    /// Provides the skeleton of the game. The managers to be used are first configured and then added
    /// to this manager. Both the client and server will use this manager. The game loop and threads are
    /// all handled here.
    class game_manager {
    public:
        /// Constructs this manager
        /// \param executable_path The path of the main executable
        /// \param log_manager The log manager to use
        /// \param window_manager The window manager to use
        /// \param graphics_manager The graphics manager to use
        /// \param scene_manager The scene manager to use
        game_manager(std::filesystem::path executable_path,
                     std::shared_ptr<apis::logging::ilog_manager> log_manager,
                     std::shared_ptr<apis::windowing::iwindow_manager> window_manager,
                     std::shared_ptr<apis::graphics::igraphics_manager> graphics_manager,
                     std::shared_ptr<scene::iscene_manager> scene_manager)
            : _executable_path(executable_path),
              _log_manager(log_manager),
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

        /// Runs the game. This is a blocking function. It will exit when the game exits.
        /// Other needed threads, such as the graphics thread, are started in this function.
        /// \returns `true` upon success, else `false`
        [[nodiscard]]
        bool run() noexcept;

    private:
        /// The path of the main executable
        std::filesystem::path _executable_path;

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
        std::atomic_bool _has_exit_been_requested { false };

        /// Used to synchronize the graphics thread with the main logic thread
        std::binary_semaphore _graphics_synchronize_semaphore { 0 };

        /// Shuts down the game
        /// \returns `true` upon success, else `false`
        [[nodiscard]]
        bool shutdown() noexcept;

        /// Requests this game manager exits
        void request_exit() noexcept;

        /// Sets up a new frame
        void begin_frame() noexcept;

        /// Updates any frame logic
        /// \returns `true` upon success, else `false`
        [[nodiscard]]
        bool update_frame() noexcept;

        /// Enters the frame synchronize state so data can be submitted to other threads
        void enter_synchronize_frame() noexcept;

        /// Synchronizes data with other threads, such as submitting renderable entities to
        /// the graphics manager
        void synchronize_frame() noexcept;

        /// Exits the frame synchronize state
        void exit_synchronize_frame() noexcept;

        /// Runs the graphics manager on a separate thread.
        /// This function will only exit if `_has_exit_been_requested` is `true`.
        /// All access to the graphics manager should only occur after `enter_synchronize_frame()`
        /// is called and before `exit_synchronize_frame()` is called.
        /// \param graphics_manager The graphics manager to run
        /// \param has_exit_been_requested Will be set to `true` if this thread should exit
        /// \param graphics_synchronize_semaphore Used to synchronize the graphics thread with the logic thread
        static void run_graphics_manager(std::shared_ptr<apis::graphics::igraphics_manager> graphics_manager,
                                         std::atomic_bool& has_exit_been_requested,
                                         std::binary_semaphore& graphics_synchronize_semaphore) noexcept;
    };
}
