# Entity Component System

All entities in the game are managed by the ECS (entity component system). The client ECS acts on the data it sees. Entities that require their data updated from the server are updated by an ECS process.

- [Entity Component System](#entity-component-system)
  - [location_2d](#location_2d)
  - [location_3d](#location_3d)
  - [sprite](#sprite)
  - [mesh](#mesh)
  - [material](#material)
  - [rigid_body_2d](#rigid_body_2d)
  - [rigid_body_3d](#rigid_body_3d)
  - [velocity_2d](#velocity_2d)
  - [velocity_3d](#velocity_3d)
  - [script](#script)

The following components will be used.

## location_2d

Contains the 2D location of this entity in screen space.

## location_3d

Contains the 3D location of this entity in the world.

An array of potential anchor points will also be provided. These anchor points can be used by other entities to provide a point of anchor, such as a camera. Each anchor will be an offset from the location in this entity.

## sprite

Contains the data needed to render a 2D sprite. Requires a `location_2d` component.

## mesh

Contains the data needed to render a 3D mesh. If a `location_3d` component is attached, this mesh is rendered in world space. If a `location_2d` location is attached, the mesh is rendered in screen space.

## material

Contains the needed material data to render this entity, such as shader and textures.

## rigid_body_2d

Contains the needed data to handle 2D collisions, such as bounding box and circle collisions.

## rigid_body_3d

Contains the needed data to handle 3D collisions, such as sphere and AABB collisions.

## velocity_2d

Contains the needed data to impart movement to the attached `location_2d` component.

## velocity_3d

Contains the needed data to impart movement to the attached `location_3d` component.

## script

Contains the data needed to run a script that will act on this entity in some way. This will be used extensively for NPCs to run their AI.

See [here](ai.md) for more information on how AI is managed.
