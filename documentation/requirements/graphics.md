# Graphics

- [Graphics](#graphics)
  - [2D Graphics](#2d-graphics)
  - [3D Graphics](#3d-graphics)
  - [Materials](#materials)
    - [Material File](#material-file)
  - [Cameras](#cameras)
  - [Lighting](#lighting)

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

## Lighting

The following lighting types must be supported:

- Ambient
- Directional
- Point
- Spot

Point and spot lights can have light cookies attached.

All lights can be colored (RGB).
