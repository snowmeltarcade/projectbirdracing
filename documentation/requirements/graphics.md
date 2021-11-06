# Graphics

- [Graphics](#graphics)
  - [2D Graphics](#2d-graphics)
  - [3D Graphics](#3d-graphics)
  - [Materials](#materials)
    - [Material File](#material-file)
  - [Cameras](#cameras)
  - [Lights](#lights)

Both 2D and 3D rendering must be supported. Rendering should take place in its own thread. Data to be rendered should be pushed into this component and not read from other components.

## 2D Graphics

Rendering is done in screen space. Animated sprites, tile maps, fonts, particles and special effects are required.

## 3D Graphics

Ability to render both in screen space and world space is required. Height maps, static models, skeletal animation, particles, special effects, billboards, fluid, animated textures and reflections should be supported.

## Materials

All renderable entities require a material.

A material describes how a renderable entity should be rendered. It describes the following attributes:

* Color (RGBA)
* Textures
* Shaders
  * Fragment
  * Vertex

### Material File

A JSON file with the following format:

## Cameras

A camera is used to render a viewport to a target.

Cameras can be set to render any one of these three channels, in any combination:

* Normal
* Depth
* Color

Post processing operations must be attachable to a camera.

The following post processing effects must be supported:

* Bloom
* HDR

## Lights
