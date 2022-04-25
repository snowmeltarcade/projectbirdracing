#pragma once

#include "irender_target.h"
#include "shader_manager.h"
#include "shader_program.h"
#include "mesh.h"

#include <vector>
#include <memory>
#include <cassert>

namespace pbr::shared::apis::graphics::opengl {
    /// Takes submitted render targets and outputs them to a single render target
    class compositor final {
    public:
        /// Creates this compositor
        /// \param destination The render target to render the composed render targets
        /// \param shader_manager The shader manager
        /// \param log_manager The log manager
        compositor(std::shared_ptr<irender_target> destination,
                   const std::shared_ptr<shader_manager>& shader_manager,
                   const std::shared_ptr<logging::ilog_manager>& log_manager)
            : _destination(destination) {
            assert((destination));

            if (!this->load_resources(shader_manager, log_manager)) {
                log_manager->log_message("Failed to load resources for compositor.",
                                         logging::log_levels::error,
                                         "Graphics");
            }
        }

        /// Destroys this compositor
        ~compositor() = default;

        /// Renders the passed render targets to the destination render target
        /// \param submitted_targets The targets to render
        void render(const std::vector<std::shared_ptr<irender_target>>& submitted_targets) noexcept;

    private:
        /// The destination render target
        std::shared_ptr<irender_target> _destination;

        /// The shader program to use
        std::shared_ptr<shader_program> _shader_program;

        /// The mesh to render against
        std::shared_ptr<mesh> _mesh;

        /// Loads the resources needed for this compositor
        /// \param shader_manager The shader manager
        /// \param log_manager The log manager
        /// \returns `true` upon success, else `false`
        [[nodiscard]]
        bool load_resources(const std::shared_ptr<shader_manager>& shader_manager,
                            const std::shared_ptr<logging::ilog_manager>& log_manager) noexcept;
    };
}
