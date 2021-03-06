#pragma once

namespace pbr::shared::apis::windowing {
    /// Provides an interface to a console window. A console window outputs standard output channels, such
    /// as `stdout`, `stderr` etc.... Using this window after the window manager has shutdown is undefined
    /// behavior
    class iconsole_window {
    public:
        iconsole_window() = default;
        virtual ~iconsole_window() = default;
    };
}
