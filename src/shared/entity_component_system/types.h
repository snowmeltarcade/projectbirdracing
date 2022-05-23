#include <cstdint>

namespace pbr::shared::entity_component_system {
    /// An entity id. This is the index into the ecs array that the entity
    /// resides.
    typedef uint32_t entity_id;
}