#pragma once

#include <memory>
#include <string>

#include "shared/data/data_manager.h"
#include "shared/apis/logging/ilog_manager.h"
#include "shared/apis/windowing/iwindow_manager.h"
#include "igraphics_manager.h"
#include "application_information.h"
#include "performance_settings.h"

namespace pbr::shared::apis::graphics {
    /// Creates a graphics manager based on the graphics config
    class graphics_manager_factory {
    public:
        /// Creates a graphics manager based on the graphics config
        /// \param data_manager The data manager
        /// \param log_manager The log manager to use
        /// \param graphics_log_manager The log manager to use for the graphics manager
        /// \param window_manager The window manager
        /// \param application_information Needed application information
        /// \param performance_settings The performance settings to use
        /// \param config_path The config path
        /// \returns The graphics manager if the config is valid, else `nullptr`
        [[nodiscard]]
        static std::shared_ptr<igraphics_manager> create(
            const std::shared_ptr<data::data_manager>& data_manager,
            const std::shared_ptr<logging::ilog_manager>& log_manager,
            const std::shared_ptr<logging::ilog_manager>& graphics_log_manager,
            const std::shared_ptr<windowing::iwindow_manager>& window_manager,
            const application_information& application_information,
            const performance_settings& performance_settings,
            const std::filesystem::path& config_path = graphics_manager_factory::graphics_config_path) noexcept;

    private:
        /// The path to the graphics config
        static const std::filesystem::path graphics_config_path;
    };
}
