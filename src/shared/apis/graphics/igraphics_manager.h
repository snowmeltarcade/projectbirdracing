#pragma once

#include <filesystem>

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
        virtual bool load_api(const std::filesystem::path& executable_path) = 0;

        /// Initializes this manager
        /// \returns `true` upon success, else `false`
        [[nodiscard]]
        virtual bool initialize() = 0;
    };
}
