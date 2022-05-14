# Entity Component System

All entities in the game are managed by the ECS (entity component system). The client ECS acts on the data it sees. Entities that require their data updated from the server are updated by an ECS process.

- [Entity Component System](#entity-component-system)
  - [Repositories](#repositories)
  - [Thread Safety](#thread-safety)
  - [Components](#components)
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
    - [camera](#camera)
  - [Types](#types)
    - [render_system](#render_system)

## Repositories

All components are stored in a repository. This is queried by systems to return the components to act on.

## Thread Safety

Component data is not thread safe, thus cannot be read from and written to concurrently. We want to avoid locks and sync points as much as possible.

Systems that require write access to the component data must be run sequentially on the game loop thread. When they are processing data, they can scale horizontally by creating multiple worker threads to process the data concurrently, as they have sole access to the data. Systems that only read component data can request a copy of the needed data, which they can then copy to a separate thread for processing. The data copy operation will be performed on the main thread in a mutex specific for that system.

The following table describes which systems need which type of data access.

| System Name | Access Type |
| -- | -- |
| Graphics | Read Only |
| Audio | Read Only |
| Physics | Read/Write |
| Network (Outbound) | Read Only |
| Network (Inbound) | Read/Write |

## Components

The following components will be used.

### location_2d

Contains the 2D location of this entity in screen space.

The parameters are as follows:

| Name | Type | Description |
| -- | -- | -- |
| `x` | `single` | The x position. `0.0f` is left of screen space and `1.0f` is the right of screen space. |
| `y` | `single` | The y position. `0.0f` is top of screen space and `1.0f` is the bottom of screen space. |

### location_3d

Contains the 3D location of this entity in the world.

The parameters are as follows:

| Name | Type | Description |
| -- | -- | -- |
| `x` | `single` | The x position in world space |
| `y` | `single` | The y position in world space |
| `z` | `single` | The z position in world space |

### sprite

Contains the data needed to render a 2D sprite. Requires a `location_2d` component.

### mesh

Contains the data needed to render a 3D mesh. If a `location_3d` component is attached, this mesh is rendered in world space. If a `location_2d` location is attached, the mesh is rendered in screen space.

### material

Contains the needed material data to render this entity, such as shader and textures.

### rigid_body_2d

Contains the needed data to handle 2D collisions, such as bounding box and circle collisions.

### rigid_body_3d

Contains the needed data to handle 3D collisions, such as sphere and AABB collisions.

### velocity_2d

Contains the needed data to impart movement to the attached `location_2d` component.

### velocity_3d

Contains the needed data to impart movement to the attached `location_3d` component.

### script

Contains the data needed to run a script that will act on this entity in some way. This will be used extensively for NPCs to run their AI.

See [here](ai.md) for more information on how AI is managed.

### camera

Renders its view to a render target.

The parameters are as follows:

| Name | Type | Description |
| -- | -- | -- |
| `name` | `string` | The name of this camera |
| `render_target_index` | `uint?` | The index of the render target to render to |
| `render_system` | [`render_system`](#render_system) | The render system |
| `layer_index` | `uint?` | The index of the layer to render. `0` is counted as closest to the screen, with higher indexes being counted as farther away. Layer indexes are only used for `screen` render targets. |

## Types

The types used in the ECS are as follows.

### render_system

Represents a [render system](../technical_architecture/graphics.md#render-systems).

Valid `enum` values are as follows:

| Name | Description |
| -- | -- |
| `2d` | The 2D render system |
| `3d` | The 3D render system |
