# Graphics

- [Graphics](#graphics)
  - [Materials](#materials)
    - [Material Update Script](#material-update-script)
  - [Algorithms](#algorithms)
    - [Edge Detection](#edge-detection)
    - [Shadows](#shadows)
    - [Lighting](#lighting)
  - [Texturing](#texturing)
    - [Texture Names](#texture-names)
  - [Cameras](#cameras)
    - [Named Texture Render Targets](#named-texture-render-targets)

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

## Texturing

2D textures will be used for all texture channels.

The following image formats are supported for texture data:

| Format |
| --- |
| PNG |

The following color formats are required for the channels:

| Channel | Format |
| --- | --- |
| Albedo | RGBA |
| Normal | RGB |
| Specular | BW |

### Texture Names

Textures are refereed to by their names, not their file paths.

The mapping between name and file path is defined in the `data/graphics/textures/textures.json` file. It has the following format:

```json
{
  "textures": [
    {
      "name": "texture name",
      "file_path": "path [optional]",
      "width": "default|wpx (where w = width in pixels)",
      "height": "default|hpx (where h = height in pixels)",
      "default_color": "color name|#FFFFFFFF [optional]"
    }
  ]
}
```

`name` is the name of the texture.

`file_path` is relative to the `data/graphics/textures/` directory. Duplicate names will cause an error. Duplicate file paths are allowed. The resource manager will ensure that duplicate files, at the same path, are not loaded into memory more than once.

`width` is the width in pixels the texture should be in memory. If loaded from an image, the texture will be scaled to this size.

`height` is the height in pixels the texture should be in memory. If loaded from an image, the texture will be scaled to this size.

`default_color` is the default color of this texture. If the texture is a render target, this will be the color before anything is rendered to it. This color will not affect the color of the texture if loaded from a file or when rendered to from a camera.

## Cameras

Cameras will always render to a target. A target will either be a screen surface, an in-memory texture or a named texture.

### Named Texture Render Targets

If a named texture is rendered to, the material using that texture will display the results of that render.

When a camera renders to a named texture, if the named texture has an associated file that was loaded against it, the file will be disassociated and the rendered texture will take its place. The size of this new named texture will default to the size of the camera's viewport. If the named texture does not exist, it will be created and the size will be the same as the camera's viewport. If it does exist and is not associated with a file, the render will be scaled to the texture's size.
