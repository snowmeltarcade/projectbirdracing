#pragma once

namespace pbr::shared::apis::graphics {
    /// Provides an interface to the graphics manager. This manages the graphics API instance and rendering
    /// processes.
    class igraphics_manager {
    public:
        /// Constructs this manager
        igraphics_manager() = default;

        /// Destroys this manager
        virtual ~igraphics_manager() = default;

        /// Initializes this manager
        /// \returns `true` upon success, else `false`
        [[nodiscard]]
        virtual bool initialize() = 0;
    };
}
