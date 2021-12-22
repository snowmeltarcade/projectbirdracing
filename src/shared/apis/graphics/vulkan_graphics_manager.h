#pragma once

#include "shared/apis/logging/ilog_manager.h"
#include "shared/apis/windowing/iwindow_manager.h"
#include "igraphics_manager.h"
#include "application_information.h"
#include "vulkan/instance.h"

#include <memory>
#include <string>

namespace pbr::shared::apis::graphics {
    /// Handles the Vulkan graphics API and rendering processes
    class vulkan_graphics_manager : public igraphics_manager {
    public:
        /// Constructs this manager
        /// \param log_manager The log manager to use
        /// \param window_manager The window manager
        /// \param application_information Needed application information
        vulkan_graphics_manager(std::shared_ptr<apis::logging::ilog_manager> log_manager,
                                std::shared_ptr<apis::windowing::iwindow_manager> window_manager,
                                application_information application_information)
            : _log_manager(log_manager),
                _window_manager(window_manager),
                _application_information(application_information)
        {
        }

        /// Destroys this manager
        ~vulkan_graphics_manager() override = default;

        /// Initializes this manager
        /// \returns `true` upon success, else `false`
        [[nodiscard]]
        bool initialize() override;

    private:
        /// The log manager
        std::shared_ptr<apis::logging::ilog_manager> _log_manager;

        /// The window manager
        std::shared_ptr<apis::windowing::iwindow_manager> _window_manager;

        /// Needed application information
        application_information _application_information;

        /// The Vulkan instance
        vulkan::instance _instance;
    };
}
