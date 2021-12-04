#pragma once

#include "iconsole_window.h"

namespace pbr::shared::apis::windowing {
    /// A console window outputs standard output channels, such as `stdout`, `stderr` etc...
    class console_window : public iconsole_window {
    public:
        console_window() = default;
        ~console_window() override = default;
    };
}
