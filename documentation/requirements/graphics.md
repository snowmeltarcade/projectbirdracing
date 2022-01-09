# Graphics

- [Graphics](#graphics)
  - [2D Graphics](#2d-graphics)
  - [3D Graphics](#3d-graphics)
  - [Materials](#materials)
    - [Material File](#material-file)
  - [Cameras](#cameras)
    - [Tripods](#tripods)
  - [Lighting](#lighting)
  - [Performance Settings Profiles](#performance-settings-profiles)

Both 2D and 3D rendering must be supported. Rendering should take place in its own thread. Data to be rendered should be pushed into this component and not read from other components.

## 2D Graphics

Rendering is done in screen space. Animated sprites, tile maps, fonts, particles and special effects are required.

## 3D Graphics

Ability to render both in screen space and world space is required. Height maps, static models, skeletal animation, particles, special effects, billboards, fluid, animated textures and reflections should be supported.

## Materials

All renderable entities require a material.

A material describes how a renderable entity should be rendered. It describes the following attributes:

- Color (RGBA)
- Textures
- Shaders
  - Fragment
  - Vertex

The following channels can be set using textures:

- Albedo
- Normal
- Specular

Materials can be updated by means of a script. This will allow animated textures or animated materials. The update function time is defined in the material file.

### Material File

A JSON file with the following format:

```json
{
  "name": "material name",
  "color": "color name or #AABBCCDD [default=#FFFFFFFF]",
  "albedo": "albedo texture name [optional]",
  "normal": "normal texture name [optional]",
  "specular": "specular texture name [optional]",
  "vertex_shader": "vertex shader name [optional]",
  "fragment_shader": "fragment shader name [optional]",
  "update_time": "frame|tms (where t = integer > 0) [optional][default=0]",
  "update_script_name": "update script name [optional]"
}
```

## Cameras

A camera is used to render a viewport to a target.

Cameras can be set to render any one of these three channels, in any combination:

- Normal
- Depth
- Color

Post processing operations must be attachable to a camera.

The following post processing effects must be supported:

- Bloom
- HDR

### Tripods

A camera will be attached to a tripod. A tripod will provide the location and orientation of the camera. The tripod can be attached to another entity, via an attachment point on that entity, thus the tripod will inherit the entity's location and orientation.

A tripod can also have a distance, which acts as an offset from the [anchor point](./entity_component_system.md#location_3d), much like a camera at the end of a selfie stick.

If a camera is set to look at an entity, the required look at orientation will override any orientation provided by the tripod.

## Lighting

The following lighting types must be supported:

- Ambient
- Directional
- Point
- Spot

Point and spot lights can have light cookies attached.

All lights can be colored (RGB).

## Performance Settings Profiles

Various graphics details and performance settings are user configurable. The settings profiles will be saved in a `JSON` file in the `data/graphics/performance` folder. All settings will have defaults in code. These defaults are API and device specific, so are implementation defined. The user can select which profile to use in the graphics menu.

The configurable settings are as follows:

| Name | Values | Description |
| -- | -- | -- |
| `prefer_vsync` | `true`/`false` | If `true`, vsync will be used |
| `msaa_samples` | `1`, `2`, `4`, `8`, `16`, `32`, `64` | The number of [MSAA](https://en.wikipedia.org/wiki/Multisample_anti-aliasing) samples to use. If the device does not support the requested number of samples, the nearest number will be used. |
| `max_frames_in_flight` | `1`, `2`, `3` | The maximum number of frames the GPU will process at concurrently |
| `sample_shading` | `0.0` to `1.0` | Similar to MSAA, but samples pixels within a triangle. The closer to `1.0`, the smoother the pixels. A value of `0.0` will disable this setting. |

The settings profile `JSON` file will have the following format. The keys are the named values in the above table. For example:

```json
{
  "prefer_vsync": "false",
  "msaa_samples": 32
}
```
