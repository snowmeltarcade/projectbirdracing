# Server

- [Server](#server)
  - [Graphics](#graphics)
  - [Window](#window)
  - [Input](#input)
  - [Networking](#networking)
  - [Scene](#scene)
  - [World](#world)
    - [World Generation](#world-generation)
      - [World Data Files](#world-data-files)
      - [Biomes](#biomes)
    - [World Simulations](#world-simulations)
  - [Physics](#physics)
    - [2D Physics](#2d-physics)
  - [UI](#ui)
  - [ECS](#ecs)
  - [Scripting](#scripting)
  - [Data](#data)
  - [Game Logic](#game-logic)
    - [Avions](#avions)
    - [Mining](#mining)
    - [Farming](#farming)
    - [Breeding](#breeding)
      - [Breeding Avions](#breeding-avions)
      - [Breeding Crops](#breeding-crops)
  - [Component Architecture](#component-architecture)

All game logic and data is handled by the server.

The server has these components - graphics, window, input, networking, scene, world, physics, UI (User Interface), ECS (Entity Component System) and scripting.

Where possible, each component should work in its own thread and work independently of other components.

## Graphics

Basic 2D graphics are required to display a UI and the current world.

See [here](graphics.md) for more information.

## Window

Ability to create game window required. No need for full screen, but the window needs to be resizable.

## Input

Keyboard, mouse, and touch screen support is required. The input source should be abstracted into actions, which is what the game system will act on.

Input can be handled in the same thread as the windowing system.

## Networking

TCP and UDP must be supported. Network packets that must be received by the client should be sent via TCP. Packets that are not required should be sent over UDP.

UDP packets will be sent with a timestamp. This will help the client discard packets that are no longer up-to-date.

## Scene

Manages the current scene the user is in. A scene may or may not contain a world. The required scenes are:

- Loading
  - Handles the loading of worlds, various components and displays the loading UIs
- World
  - Contains the current running world and the UIs

## World

The world on the server represents the most up-to-date state of truth. It contains all entities and terrain data associated with the world.

A 2D grid is used to model the world. Tiles are used to indicate what the terrain is at that location. All entities have a 2D coordinate that indicates their location in the world. Static objects are stored in the world that are indicated by a type and a location.

Any changes to the tiles in the world are sent to the client as a required network packet, as this data only needs to be sent once. Entity updates are sent to the client as non-required network packets, as only the most up-to-date version of the state is useful.

All worlds are stored on the server as separate instances. All world instances must be self contained and not rely on data from any other world. Avatars and players can only be part of one world at a time. To assist with world management and transporting avatars and players from one world to another, a world manager will be used.

World tiles have attributes and types. Attributes indicate how avatars can interact with them and types indicate a display and function of a tile.

The following are the tile attributes:

- `runnable`
  - An avatar can run on this tile
- `walkable`
  - An avatar can walk on this tile
- `plantable`
  - Crops can be planted on this tile
- `teleport`
  - Teleport an avatar to another world
- `trigger`
  - Triggers a scripted action

The following are the tile types:

- `grass`
  - A patch of grass. Will appear as dry ground.
- `beach`
  - A segment of beach. Will appear in between water and `grass`.
- `rock`
  - A segment of rock. Will appear on steeper inclines.
- `mud`
  - A patch of mud. Will appear where `grass` has been tilled, ready for farming.
- `water_shallow`
  - Shallow water. Will appear under shallow water, such as rivers.
- `water_deep`
  - Deep water. Will appear under deep water, such as oceans.

### World Generation

When the server starts, a random world is generated, or loaded if world data has been saved. A pseudo-random number generator (PRNG) will be used to generate the world. After a world has been generated, the PRNG's seed and the world's data will be saved.

Each world tile will represent a 1m x 1m square. The generated main world will be 10000x10000 tiles, and a generated race map will be 1000x1000 tiles.

A world consists of a base, or default, biome. Throughout the world, multiple other biomes will be generated.

Villages will be randomly placed in a main world. Each main world will have 25 villages, with a minimum of 1km distance between each village. Different biomes will have a different chance of villages being generated within them.

#### World Data Files

The world data file will be stored in `data/worlds/` with the filename being `the_world_name.json`. The name of the world will be entered by the user of the server as part of the world generation process.

The file structure will be as follows:

```json
{
  "seed": 123456,
  "width": 10000,
  "height": 10000,
  "tiles": [ 0, 1, 2, 3, ]
}
```

`seed` is the PRNG's seed.

`width` is the world's width in tiles.

`height` is the world's height in tiles.

`tiles` specifies the tile indexes used for the world. The size of data is `width x height` in a single array. If `tiles` is an incorrect size, an error will occur.

Data about world tiles are stored in the file `data/world/tiles.json`. It has the following structure:

```json
{
  "tiles": [
    {
      "id": 1,
      "attributes": [ "runnable", "walkable" ],
      "type": "grass",
      "texture_name": "texture_name",
      // TODO: Audio names? Per attribute, one sound for running, one for walking?
    }
  ]
}
```

`tiles` is the list of available.

`id` is the id of the tile. This must be unique for all tiles. If a duplicate id is found, an error will occur.

`attributes` specifies the attributes this tile has. See [here](#world) for more information.

`type` specifies the type of this tile. See [here](#world) for more information.

`texture_name` is the name of the texture that this tile should render with.

#### Biomes

A biome will define the types of tiles are generated and also what minerals, crops and avions can be found there. Weather is also defined by a biome. Differing weather will affect avions as they race.

See [here](world_generation_biomes.md) for more information about biomes.

### World Simulations

There are various simulations that are run. See [here](world_simulations.md) for more information.

## Physics

2D is required.

### 2D Physics

Bounding box and circle collision, translation, rotation, dilation, reflection are required, along with acceleration and gravity. This will be used by the UI system and 2D particles.

## UI

See [UI](ui.md) for more information.

## ECS

All entities in the server are managed by the ECS (entity component system).

The following components are required:

- `location_2d`
  - Contains the 2D location of this entity in screen space
- `sprite`
  - Contains the data needed to render a 2D sprite. Requires a `location_2d` component.
- `material`
  - Contains the needed material data to render this entity, such as shader and textures.
- `rigid_body_2d`
  - Contains the needed data to handle 2D collisions, such as bounding box and circle collisions.
- `velocity_2d`
  - Contains the needed data to impart movement to the attached `location_2d` component.
- `script`
  - Contains the data needed to run a script that will act on this entity in some way. This will be used extensively for NPCs to run their AI.

See [here](ai.md) for more information on how AI is managed.

## Scripting

Customizable workflows will be handled by scripting. These workflows are:

- UI events
- World tile triggers
- Avion AI behaviors
- Farming and mining tool behaviors

## Data

All external data should be stored in a folder called `data` that is in the same directory as the server's executable.

## Game Logic

All game logic is to be handled on the server.

### Avions

### Mining

### Farming

### Breeding

Avions and crops can be bred. When bred, the attributes of the parents are merged and passed to the child using a genetic algorithm.

#### Breeding Avions

#### Breeding Crops

Seeds can be bred to create new types of seed.

## Component Architecture

This is a diagram describing the overall architecture of the server:

![Overall Server Architecture](images/server_architecture.drawio.png)
