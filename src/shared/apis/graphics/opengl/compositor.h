#pragma once

#include "irender_target.h"
#include "shader_manager.h"

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
        compositor(std::shared_ptr<irender_target> destination,
                   const std::shared_ptr<shader_manager>& shader_manager)
            : _destination(destination) {
            assert((destination));

            auto _ = shader_manager->get("compositor");
        }
        /// Destroys this compositor
        ~compositor() = default;

        /// Renders the passed render targets to the destination render target
        /// \param submitted_targets The targets to render
        void render(const std::vector<std::shared_ptr<irender_target>>& submitted_targets) noexcept;

    private:
        /// The destination render target
        std::shared_ptr<irender_target> _destination;
    };
}
