#pragma once

#include "shared/apis/logging/ilog_manager.h"
#include "shared/apis/windowing/iwindow_manager.h"
#include "igraphics_manager.h"
#include "application_information.h"
#include "vulkan/instance.h"
#include "vulkan/window_surface.h"
#include "vulkan/physical_device.h"
#include "vulkan/device.h"
#include "vulkan/vma.h"

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
        ~vulkan_graphics_manager() override {
            if (!this->shutdown()) {
                this->_log_manager->log_message("Failed to shutdown the graphics manager.",
                                                apis::logging::log_levels::error,
                                                "Graphics");
            }
        }

        /// Loads any needed dependencies or libraries required by the graphics api
        /// \param executable_path The path of the main executable
        /// \returns `true` upon success, else `false`
        [[nodiscard]]
        bool load_api(const std::filesystem::path& executable_path) override;

        /// Initializes this manager
        /// \returns `true` upon success, else `false`
        [[nodiscard]]
        bool initialize() override;

    private:
        /// Sets the needed environment variables for Vulkan if they are not already set by the developer
        /// \param executable_path The path of the main executable
        void set_environment_variables(const std::filesystem::path& executable_path) const noexcept;

        /// Shuts down the graphics manager
        /// \returns `true` upon success, else `false`
        [[nodiscard]]
        bool shutdown() noexcept;

        /// The log manager
        std::shared_ptr<apis::logging::ilog_manager> _log_manager;

        /// The window manager
        std::shared_ptr<apis::windowing::iwindow_manager> _window_manager;

        /// Needed application information
        application_information _application_information;

        /// The Vulkan instance
        vulkan::instance _instance { this->_log_manager };

        /// The window surface to render to
        std::unique_ptr<vulkan::window_surface> _window_surface;

        /// The physical device to use for graphics processing
        std::unique_ptr<vulkan::physical_device> _physical_device;

        /// The logical device to use for graphics processing
        std::unique_ptr<vulkan::device> _device;

        /// Manages VMA
        std::unique_ptr<vulkan::vma> _vma;
    };
}
