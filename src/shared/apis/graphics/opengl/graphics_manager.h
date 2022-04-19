#pragma once

#include "shared/apis/logging/ilog_manager.h"
#include "shared/apis/windowing/iwindow_manager.h"
#include "shared/apis/graphics/igraphics_manager.h"
#include "shared/apis/graphics/application_information.h"
#include "shared/apis/graphics/performance_settings.h"
#include "context.h"

namespace pbr::shared::apis::graphics::opengl {
    /// Handles the OpenGL graphics API and rendering processes
    class graphics_manager final : public igraphics_manager {
    public:
        /// Constructs this manager
        /// \param log_manager The log manager to use
        /// \param window_manager The window manager
        /// \param application_information Needed application information
        /// \param performance_settings The performance settings to use
        graphics_manager(std::shared_ptr<logging::ilog_manager> log_manager,
                         std::shared_ptr<windowing::iwindow_manager> window_manager,
                         application_information application_information,
                         performance_settings performance_settings)
            : _log_manager(log_manager),
                _window_manager(window_manager),
                _application_information(application_information),
                _performance_settings(performance_settings) {
        }

        /// Destroys this manager
        ~graphics_manager() override {
            if (!this->shutdown()) {
                this->_log_manager->log_message("Failed to shutdown the graphics manager.",
                                                logging::log_levels::error,
                                                "Graphics");
            }
        }

        /// Returns the api implemented by this manager
        /// \returns The api implemented by this manager
        [[nodiscard]]
        apis implemented_api() const noexcept override {
            return apis::opengl;
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

        /// Submits the passed entities to be rendered by this frame
        /// \param renderable_entities The entities to render
        void submit_renderable_entities(renderable_entities renderable_entities) noexcept override;

        /// Submits a frame for rendering.
        /// All previously submitted renderable entities will be cleared after this frame is
        /// submitted
        void submit_frame_for_render() noexcept override;

        /// Returns if this graphics manager should run on a separate thread or not
        /// \returns `true` if this should run on a separate thread, else `false`
        [[nodiscard]]
        bool run_on_separate_thread() const noexcept override {
            return false;
        }

    private:
        /// The log manager
        std::shared_ptr<logging::ilog_manager> _log_manager;

        /// The window manager
        std::shared_ptr<windowing::iwindow_manager> _window_manager;

        /// The OpenGL context
        std::shared_ptr<context> _context;

        /// Needed application information
        application_information _application_information;

        /// The performance settings to use
        performance_settings _performance_settings;

        /// The entities to render
        renderable_entities _renderable_entities;

        /// Shuts down the graphics manager
        /// \returns `true` upon success, else `false`
        [[nodiscard]]
        bool shutdown() noexcept;

        /// Sets up GLEW
        /// \returns `true` upon success, else `false`
        [[nodiscard]]
        bool setup_glew() const noexcept;
    };
}
