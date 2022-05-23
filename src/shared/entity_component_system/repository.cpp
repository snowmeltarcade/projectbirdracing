#include "repository.h"

namespace pbr::shared::entity_component_system {
    entity_id repository::add_camera(components::camera camera, components::location_3d location) noexcept {
        return this->push_entity(camera, {}, location);
    }

    void repository::remove_entity(entity_id id) noexcept {
        assert((this->_in_use.size() > id));

        this->_in_use[id] = false;

        this->set_entity(id, {}, {}, {});
    }

    entity_id repository::push_entity(components::camera camera,
                                      components::location_2d location_2d,
                                      components::location_3d location_3d) noexcept {
        auto unused_index = std::find_if(this->_in_use.begin(),
                                         this->_in_use.end(),
                                         [](const auto& index){ return !index; });

        // all indexes are being used, so add a new index
        if (unused_index == this->_in_use.end()) {
            this->_cameras.emplace_back(std::move(camera));
            this->_location_2ds.emplace_back(std::move(location_2d));
            this->_location_3ds.emplace_back(std::move(location_3d));

            this->_in_use.emplace_back(true);
            return this->_in_use.size() - 1;
        } else {
            auto index = std::distance(this->_in_use.begin(), unused_index);

            // replace the first unused index
            this->set_entity(index,
                             camera,
                             location_2d,
                             location_3d);

            this->_in_use[index] = true;

            return index;
        }
    }

    void repository::set_entity(entity_id id,
                                components::camera camera,
                                components::location_2d location_2d,
                                components::location_3d location_3d) noexcept {
        this->_cameras[id] = camera;
        this->_location_2ds[id] = location_2d;
        this->_location_3ds[id] = location_3d;
    }
}
