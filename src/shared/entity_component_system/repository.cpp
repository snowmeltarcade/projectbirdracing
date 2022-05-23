#include "repository.h"

namespace pbr::shared::entity_component_system {
    entity_id repository::add_camera(components::camera camera, components::location_3d location) noexcept {
        return this->push_entity(camera, {}, location);
    }

    void repository::remove_entity(entity_id id) noexcept {
        assert((this->_in_use.size() > id));

        this->_in_use[id] = false;
    }

    entity_id repository::push_entity(components::camera camera,
                                      components::location_2d location_2d,
                                      components::location_3d location_3d) noexcept {
        this->_cameras.emplace_back(std::move(camera));
        this->_location_2ds.emplace_back(std::move(location_2d));
        this->_location_3ds.emplace_back(std::move(location_3d));

        this->_in_use.emplace_back(true);
        return this->_in_use.size() - 1;
    }
}
