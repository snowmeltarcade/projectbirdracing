# Scenes

A scene represents a simulation within the game. It encompasses all entities, game logic and systems needed to run the simulation.

The server contains the master representation of the scene. It manages all players connected to that scene. The client mirrors (to the best of its ability) the scene as hosted by the server.

- [Scenes](#scenes)
  - [Required Scenes](#required-scenes)
  - [Loading](#loading)
    - [Scene File Format](#scene-file-format)

## Required Scenes

The required scenes are:

- Loading
  - Handles the loading of worlds, various components and displays the loading UIs
- World
  - Contains the current running world and the UIs

## Loading

The scene manager loads scenes as needed. When a scene loads, it builds the scene from a scene file, if one is present. The filename of this file is `data/scenes/<scene type>.json`. As all scene types are known at compile time, the filename to look for is also known at compile time.

### Scene File Format

The file format to represent a scene is as follows:

```json
{
    "cameras": [
        {
            "name": "player",
            "layer": 1,
            "render_target_name": "player",
            "system": "3d",
            "position": { "x": 0.0, "y": 1.1, "z": 2.2 },
            "focus": { "x": 0.0, "y": 1.1, "z": 2.2 },
            "up": { "x": 0.0, "y": 1.1, "z": 2.2 }
        },
        {
            "name": "ui",
            "layer": 0,
            "render_target_name": "ui",
            "system": "2d",
            "position": { "x": 0.0, "y": 1.1 }
        },
        {
            "name": "stadium",
            "parent": "player",
            "render_target_name": "stadium",
            "system": "3d",
            "position": { "x": 0.0, "y": 1.1, "z": 2.2 },
            "focus": { "x": 0.0, "y": 1.1, "z": 2.2 },
            "up": { "x": 0.0, "y": 1.1, "z": 2.2 }
        }
    ]
}
```

The fields are described here.

| Field Name | Type | Description |
| -- | -- | -- |
| `cameras` | `camera` | The cameras |

The `camera` type:

| Field Name | Type | Description |
| -- | -- | -- |
| `name` | `string` | The name of the camera |
| `layer` | `int?` | The render index of the camera |
| `render_target_name` | `string?` | The render target's name |
| `system` | `render_system` | The render system |
| `position` | `location` | The position |
| `focus` | `location?` | The focus of this camera. Ignored on 2d systems. |
| `up` | `location?` | The direction of `up` for this camera. Ignored on 2d systems. |

The `location` type:

| Field Name | Type | Description |
| -- | -- | -- |
| `x` | `single` | The x position |
| `y` | `single` | The y position |
| `z` | `single?` | The z position |

The `render_system` type is an enum entered as a `string`. Valid values:

| Value Name | Description |
| -- | -- | -- |
| `2d` | The 2d render system |
| `3d` | The 3d render system |
