#pragma once

#include "shared/apis/logging/ilog_manager.h"
#include "shared/apis/windowing/iwindow_manager.h"
#include "igraphics_manager.h"
#include "application_information.h"
#include "performance_settings.h"
#include "vulkan/instance.h"
#include "vulkan/window_surface.h"
#include "vulkan/physical_device.h"
#include "vulkan/device.h"
#include "vulkan/vma.h"
#include "vulkan/queue.h"
#include "vulkan/command_pool.h"
#include "vulkan/swap_chain.h"
#include "vulkan/render_pass.h"
#include "vulkan/framebuffer.h"

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
        /// \param performance_settings The performance settings to use
        vulkan_graphics_manager(std::shared_ptr<apis::logging::ilog_manager> log_manager,
                                std::shared_ptr<apis::windowing::iwindow_manager> window_manager,
                                application_information application_information,
                                performance_settings performance_settings)
            : _log_manager(log_manager),
                _window_manager(window_manager),
                _application_information(application_information),
                _performance_settings(performance_settings)
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
        bool load_api(const std::filesystem::path& executable_path) noexcept override;

        /// Initializes this manager
        /// \returns `true` upon success, else `false`
        [[nodiscard]]
        bool initialize() noexcept override;

        /// Refreshes any dynamic resources, such as swap chains, frame buffers etc...
        /// This is called both during initialization and when the window surface size changes
        /// \returns `true` upon success, else `false`
        [[nodiscard]]
        bool refresh_resources() noexcept override;

    private:
        /// Sets the needed environment variables for Vulkan if they are not already set by the developer
        /// \param executable_path The path of the main executable
        void set_environment_variables(const std::filesystem::path& executable_path) const noexcept;

        /// Shuts down the graphics manager
        /// \returns `true` upon success, else `false`
        [[nodiscard]]
        bool shutdown() noexcept;

        /// Cleans up any dynamic resources, such as the swap chain etc...
        void cleanup_resources() noexcept;

        /// The log manager
        std::shared_ptr<apis::logging::ilog_manager> _log_manager;

        /// The window manager
        std::shared_ptr<apis::windowing::iwindow_manager> _window_manager;

        /// Needed application information
        application_information _application_information;

        /// The performance settings to use
        performance_settings _performance_settings;

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

        /// The graphics queue
        std::unique_ptr<vulkan::queue> _graphics_queue;

        /// The present queue
        std::unique_ptr<vulkan::queue> _present_queue;

        /// The command pool
        std::unique_ptr<vulkan::command_pool> _command_pool;

        /// The swap chain
        std::unique_ptr<vulkan::swap_chain> _swap_chain;

        /// The render pass
        std::unique_ptr<vulkan::render_pass> _render_pass;

        /// The framebuffer
        std::unique_ptr<vulkan::framebuffer> _framebuffer;
    };
}
