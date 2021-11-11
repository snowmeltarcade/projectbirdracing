# World Generation Biomes

The following base biomes are available:

- Grass
- Snow
- Desert

Each base biome can be modified with the following modifier biome:

- Wet
- Field
- Forest
- Hilly
- Mountainous

Extra biomes that can be interspersed randomly, are as follows:

- Volcanic
  - Maximum height: `1.0`
  - Minimum height: `0.9`
  - Chance of village generation: `0%`
- Ocean
  - Maximum height: `0.1`
  - Minimum height: `0.0`
  - Chance of village generation: `0%`

## Biomes File

Biome data will be stored in `data/world/biomes.json`. It will have the following structure:

```json
{
    "biomes": [
        {
            "name": "name_of_biome",
            "chance_of_village_generation": "x%",
            "tiles": [ 0, 1, 2, 3, ]
        }
    ]
}
```

`biomes` defines the available biomes.

`name` is the name of the biome.

`chance_of_village_generation` is the percentage that a village can be generated within this biome.

`tiles` are the tile ids that will be generated in this biome. See [here](server.md#world-data-files) for more information.
