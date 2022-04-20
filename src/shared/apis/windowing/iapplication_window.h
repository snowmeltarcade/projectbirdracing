#pragma once

#include "window_size.h"

namespace pbr::shared::apis::windowing {
    /// Provides an interface to an application window. An application window can either be in windowed or
    /// full screen mode. This window will contain the render surface. Using this window after the window
    /// manager has shutdown is undefined behavior
    class iapplication_window {
    public:
        iapplication_window() = default;
        virtual ~iapplication_window() = default;

        /// Returns the window size
        /// \returns The window size
        [[nodiscard]]
        virtual window_size get_size() const noexcept = 0;

        /// Sets the window size
        /// \param width The window width
        /// \param height The window height
        /// \param fullscreen If this window should be full screen or not
        /// \returns `true` upon success, else `false`
        [[nodiscard]]
        virtual bool set_size(pixels width, pixels height, bool fullscreen) noexcept = 0;
    };
}
