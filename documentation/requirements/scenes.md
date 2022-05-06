# Scenes

A scene represents a simulation within the game. It encompasses all entities, game logic and systems needed to run the simulation.

The server contains the master representation of the scene. It manages all players connected to that scene. The client mirrors (to the best of its ability) the scene as hosted by the server.

- [Scenes](#scenes)
  - [Required Scenes](#required-scenes)
  - [File Format](#file-format)

## Required Scenes

The required scenes are:

- Loading
  - Handles the loading of worlds, various components and displays the loading UIs
- World
  - Contains the current running world and the UIs

## File Format

The file format to represent a scene is as follows:

```json
{
    "cameras": [
        {
            "name": "player",
            "layer": 1,
            "render_target_name": "player",
            "system": "3d",
            "position": { x: 0.0, y: 1.1, z: 2.2 },
            "focus": { x: 0.0, y: 1.1, z: 2.2 },
            "up": { x: 0.0, y: 1.1, z: 2.2 },
        },
        {
            "name": "ui",
            "layer": 0,
            "render_target_name": "ui",
            "system": "2d",
            "position": { x: 0.0, y: 1.1 },
        },
        {
            "name": "stadium",
            "parent": "player",
            "render_target_name": "stadium",
            "system": "3d",
            "position": { x: 0.0, y: 1.1, z: 2.2 },
            "focus": { x: 0.0, y: 1.1, z: 2.2 },
            "up": { x: 0.0, y: 1.1, z: 2.2 },
        }
    ]
}
```
