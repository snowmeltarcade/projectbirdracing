# World Generation Biomes

The following base biomes are available:

- Grass
- Snow
- Desert

Each base biome can be modified with the following modifier biome:

| Biome Name | Maximum Height | Minimum Height | Chance of Village Generation | Chance of Foliage Generation |
| --- | --- | --- | --- | --- |
| Beach | 0.15 | 0.1 | 0% | 0% |
| Field | 0.2 | 0.15 | 0% | 0% |
| Forest | 0.3 | 0.2 | 0% | 0% |
| Hilly | 0.5 | 0.3 | 0% | 0% |
| Mountainous | 1.0 | 0.5 | 0% | 0% |

Extra biomes that can be interspersed randomly, are as follows:

| Biome Name | Maximum Height | Minimum Height | Chance of Village Generation | Chance of Foliage Generation |
| --- | --- | --- | --- | --- |
| Volcanic | 1.0 | 0.5 | 0% | 0% |
| Ocean | 0.1 | 0.0 | 0% | 0% |

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
