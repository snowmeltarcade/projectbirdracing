#pragma once

#include "shared/memory/basic_allocators.h"
#include "iconsole_window.h"
#include "iapplication_window.h"

#include <memory>

namespace pbr::shared::apis::windowing {
    /// Provides an interface to the window manager. The window manager creates and manages both
    /// console and application windows
    class iwindow_manager {
    public:
        iwindow_manager() = default;
        virtual ~iwindow_manager() = default;

        /// Initializes the window manager
        /// \returns `true` upon success, else `false`
        [[nodiscard]]
        virtual bool initialize() noexcept = 0;

        /// Creates a console window
        /// \returns The created console window
        [[nodiscard]]
        virtual std::shared_ptr<iconsole_window> create_console_window() noexcept = 0;

        /// Creates an application window
        /// \returns The created application window
        [[nodiscard]]
        virtual std::shared_ptr<iapplication_window> create_application_window() noexcept = 0;

        /// Returns the main application window
        /// \returns The main application window, else `nullptr` is no application window has been created
        [[nodiscard]]
        virtual std::shared_ptr<iapplication_window> get_main_application_window() const noexcept = 0;

        /// Updates the windowing system
        /// \returns `true` upon success, else `false`
        [[nodiscard]]
        virtual bool update() noexcept = 0;

        /// Returns true if the windowing system has a quit event
        /// \returns `true` if the windowing system has a quit event, else `false`
        [[nodiscard]]
        virtual bool should_quit() const noexcept = 0;
    };
}
