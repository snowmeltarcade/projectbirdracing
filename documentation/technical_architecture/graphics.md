# Graphics

- [Graphics](#graphics)
  - [Materials](#materials)
    - [Material Update Script](#material-update-script)
  - [Algorithms](#algorithms)
    - [Edge Detection](#edge-detection)
    - [Shadows](#shadows)
    - [Lighting](#lighting)

Rendering all graphical elements and loading graphics related resources are required.

## Materials

Materials are scriptable.

### Material Update Script

The following function will be called at increments described by the `update_time` field:

- `onUpdate(material: Material)`

## Algorithms

Various rendering techniques require specific algorithms.

### Edge Detection

The [Sobel filter](https://en.wikipedia.org/wiki/Sobel_operator) will be used.

### Shadows

[Shadow depth mapping](https://learnopengl.com/Advanced-Lighting/Shadows/Shadow-Mapping) will be used.

### Lighting

[Blinn-Phong](https://en.wikipedia.org/wiki/Blinn%E2%80%93Phong_reflection_model) will be used.
