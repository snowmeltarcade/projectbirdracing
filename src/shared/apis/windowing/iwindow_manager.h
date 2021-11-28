#pragma once

#include "shared/memory/basic_allocators.h"
#include "iconsole_window.h"

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
    };
}
