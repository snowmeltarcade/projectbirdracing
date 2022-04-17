#pragma once

#include "shared/apis/logging/ilog_manager.h"
#include "shared/apis/windowing/iwindow_manager.h"
#include "shared/apis/graphics/igraphics_manager.h"
#include "shared/apis/graphics/application_information.h"
#include "performance_settings.h"
#include "instance.h"
#include "window_surface.h"
#include "physical_device.h"
#include "device.h"
#include "vma.h"
#include "queue.h"
#include "command_pool.h"
#include "command_buffer.h"
#include "swap_chain.h"
#include "render_pass.h"
#include "framebuffer.h"
#include "semaphore.h"
#include "fence.h"

#include <memory>
#include <string>
#include <shared_mutex>

namespace pbr::shared::apis::graphics::vulkan {
    /// Handles the Vulkan graphics API and rendering processes
    class graphics_manager : public igraphics_manager {
    public:
        /// Constructs this manager
        /// \param log_manager The log manager to use
        /// \param window_manager The window manager
        /// \param application_information Needed application information
        /// \param performance_settings The performance settings to use
        graphics_manager(std::shared_ptr<apis::logging::ilog_manager> log_manager,
                         std::shared_ptr<apis::windowing::iwindow_manager> window_manager,
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

        /// Submits the passed entities to be rendered by this frame
        /// \param renderable_entities The entities to render
        void submit_renderable_entities(renderable_entities renderable_entities) noexcept override;

        /// Submits a frame for rendering. In Vulkan, this simply submits a request for
        /// the frame to be rendered, the frame itself will be rendered when the driver
        /// is ready. If no color frames are free to submit a render request to, the thread
        /// will block waiting for an image to become free.
        /// All previously submitted renderable entities will be cleared after this frame is
        /// submitted
        void submit_frame_for_render() noexcept override;

    private:
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

        /// The current frame we are rendering in the frames in flight
        uint32_t _current_frame {0u};

        /// Makes sure that we do not start writing to an image before it has been acquired
        std::vector<vulkan::semaphore> _image_available_semaphores;

        /// Makes sure that we wait for a frame to finish rendering before we render to the
        /// next frame that uses the same image (we are potentially rendering multiple frames
        /// to multiple images at the same time)
        std::vector<vulkan::semaphore> _render_finished_semaphores;

        /// Makes sure the CPU waits for an image to become free to avoid overwriting an image
        /// currently being rendered to
        std::vector<vulkan::fence> _in_flight_fences;

        /// Helps avoid writing to an image if it is in flight, but was returned out of order
        /// by the driver. This just holds a pointer to the fence's native handle. The lifetime
        /// of these handles is managed by `_in_flight_fences`
        std::vector<VkFence> _images_in_flight;

        /// The command buffers to use, one for each frame submitted
        std::vector<vulkan::command_buffer> _command_buffers;

        /// Do we need to rebuild the swap chain, for instance, if the viewport changed in size
        bool _signal_swap_chain_out_of_date {false};

        /// Synchronizes submitting renderable entities
        std::shared_mutex _submit_renderable_entities_mutex;

        /// The entities to render
        renderable_entities _renderable_entities;

        /// Sets the needed environment variables for Vulkan if they are not already set by the developer
        /// \param executable_path The path of the main executable
        void set_environment_variables(const std::filesystem::path& executable_path) const noexcept;

        /// Shuts down the graphics manager
        /// \returns `true` upon success, else `false`
        [[nodiscard]]
        bool shutdown() noexcept;

        /// Cleans up any dynamic resources, such as the swap chain etc...
        void cleanup_resources() noexcept;

        /// Creates the synchronization objects needed for presenting swap chain images
        [[nodiscard]]
        bool create_swap_chain_synchronization_objects() noexcept;

        /// Creates the command buffers needed to render the submitted entities
        /// \param image_index The index of the image being rendered to in the swap chain
        /// \returns `true` upon success, else `false`
        [[nodiscard]]
        bool create_render_entities_command_buffers(uint32_t image_index) noexcept;
    };
}
