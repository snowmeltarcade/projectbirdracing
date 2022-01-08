#pragma once

#include <vector>

#include "renderable_entity_2d.h"

namespace pbr::shared::apis::graphics {
    /// Contains entities to be submitted for render
    class renderable_entities {
    public:
        /// Submits a 2d renderable entity
        void submit(renderable_entity_2d entity) noexcept;

        /// Returns the submitted 2d renderable entities
        /// \returns The submitted 2d renderable entities
        const std::vector<renderable_entity_2d>& get_2d_renderable_entities() const noexcept {
            return this->_2d_renderable_entities;
        }

    private:
        /// The 2d renderable entities to render
        std::vector<renderable_entity_2d> _2d_renderable_entities;
    };
}
