#pragma once

#include "types.h"
#include "components/camera.h"
#include "components/location_2d.h"
#include "components/location_3d.h"
#include "shared/compile_time/static_types.h"

#include <cassert>
#include <vector>
#include <type_traits>

namespace pbr::shared::entity_component_system {
    /// Stores and manages sets of component data. Entities are the indexes into the component
    /// arrays. If an entity is removed, that index is simply marked as unused. The next time
    /// an entity is added, it is inserted into that index. This allows us to not allocated more
    /// memory, unless all existing entity ids are used. This implementation wastes a lot
    /// of memory, as each entity must have an index in each component array, but is also very
    /// simple.
    class repository final {
    public:
        /// Adds a 3d camera
        /// \param camera The camera to add
        /// \param location The location of the camera
        /// \returns The entity id
        [[nodiscard]]
        entity_id add_camera(components::camera camera, components::location_3d location) noexcept;

        /// Removes the entity with the given id. This marks the entity id as not used and resets
        /// the data. It does not remove the actual index. The index will be potentially used for
        /// a future add entity operation.
        /// \param id The id of the entity to remove
        void remove_entity(entity_id id) noexcept;

        /// Gets a component for the given entity
        /// \param id The id of the entity to get the component for
        /// \returns The component
        /// \tparam T The component type to return
        template <typename T>
        [[nodiscard]]
        T get(entity_id id) {
            assert((this->_in_use.size() > id));

            if constexpr(std::is_same_v<T, components::camera>) {
                return this->_cameras[id];
            } else if constexpr(std::is_same_v<T, components::location_2d>) {
                return this->_location_2ds[id];
            } else if constexpr(std::is_same_v<T, components::location_3d>) {
                return this->_location_3ds[id];
            } else {
                static_assert(compile_time::always_false<T>::value, "T is an unsupported component.");
            }
        }

    private:
        /// Indicates if an entity is in use
        std::vector<bool> _in_use;

        /// The cameras
        std::vector<components::camera> _cameras;

        /// 2d locations
        std::vector<components::location_2d> _location_2ds;

        /// 3d locations
        std::vector<components::location_3d> _location_3ds;

        /// This pushes a new entity onto the stack
        /// \param camera The camera to add
        /// \param location_2d The 2d location
        /// \param location_3d The 3d location
        /// \returns The entity id
        [[nodiscard]]
        entity_id push_entity(components::camera camera,
                              components::location_2d location_2d,
                              components::location_3d location_3d) noexcept;

        /// Sets the given entity with the given components
        /// \param id The id of the entity to set
        /// \param camera The camera to add
        /// \param location_2d The 2d location
        /// \param location_3d The 3d location
        void set_entity(entity_id id,
                        components::camera camera,
                        components::location_2d location_2d,
                        components::location_3d location_3d) noexcept;
    };
}
