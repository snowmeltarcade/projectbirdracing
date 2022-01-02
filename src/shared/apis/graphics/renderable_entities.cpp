#include "renderable_entities.h"

namespace pbr::shared::apis::graphics {
    void renderable_entities::submit(renderable_entity_2d entity) noexcept {
        this->_2d_renderable_entities.emplace_back(std::move(entity));
    }
}