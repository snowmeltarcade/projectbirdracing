#pragma once

#include "irender_target.h"

#include <vector>
#include <memory>
#include <cassert>

namespace pbr::shared::apis::graphics::opengl {
    /// Takes submitted render targets and outputs them to a single render target
    class compositor final {
    public:
        /// Creates this compositor
        /// \param destination The render target to render the composed render targets
        compositor(std::shared_ptr<irender_target> destination)
            : _destination(destination) {
            assert((destination));
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
