#pragma once

#include <filesystem>

#include "renderable_entities.h"

namespace pbr::shared::apis::graphics {
    /// Provides an interface to the graphics manager. This manages the graphics API instance and rendering
    /// processes.
    class igraphics_manager {
    public:
        /// Constructs this manager
        igraphics_manager() = default;

        /// Destroys this manager
        virtual ~igraphics_manager() = default;

        /// Loads any needed dependencies or libraries required by the graphics api
        /// \param executable_path The path of the main executable
        /// \returns `true` upon success, else `false`
        [[nodiscard]]
        virtual bool load_api(const std::filesystem::path& executable_path) noexcept = 0;

        /// Initializes this manager
        /// \returns `true` upon success, else `false`
        [[nodiscard]]
        virtual bool initialize() noexcept = 0;

        /// Refreshes any dynamic resources, such as swap chains, frame buffers etc...
        /// This is called both during initialization and when the window surface size changes
        /// \returns `true` upon success, else `false`
        [[nodiscard]]
        virtual bool refresh_resources() noexcept = 0;

        /// Submits the passed entities to be rendered by this frame
        /// \param renderable_entities The entities to render
        virtual void submit_renderable_entities(renderable_entities renderable_entities) noexcept = 0;

        /// Submits a frame for rendering
        virtual void submit_frame_for_render() noexcept = 0;

        /// Returns if this graphics manager should run on a separate thread or not
        /// \returns `true` if this should run on a separate thread, else `false`
        [[nodiscard]]
        virtual bool run_on_separate_thread() const noexcept = 0;
    };
}
