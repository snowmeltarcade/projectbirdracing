# World Generation

- [World Generation](#world-generation)
  - [Terrain Generation](#terrain-generation)
    - [Terrain Generation Inputs](#terrain-generation-inputs)
    - [Terrain Generation Algorithm](#terrain-generation-algorithm)
      - [Generate Biome Distribution](#generate-biome-distribution)
      - [Generate Height Points](#generate-height-points)
      - [Generate Rivers](#generate-rivers)
      - [Generate Foliage](#generate-foliage)
  - [Main World Generation](#main-world-generation)
  - [Race Course Generation](#race-course-generation)
  - [Village Generation](#village-generation)

All worlds have the same terrain generation algorithm. Specific worlds will have further refinements according to their types.

## Terrain Generation

A height map will be generated in this step. The height, as defined in the alpha channel, describes the height of the tile. The values in the red channel describe the chosen biome. The blue channel describes the types of foliage at that location. The green channel will describe the locations of rivers.

### Terrain Generation Inputs

* Desired base biome
* Modifier biomes
* Extra biomes
* Ocean biome index within extra biomes
* Number of required extra biomes
* Minimum biome size in meters from center
* Maximum biome size in meters from center
* Sea level (between `0.0` and `1.0`)

### Terrain Generation Algorithm

The main phases of this algorithm are:

1. Generate biome distribution
2. Generate height points
3. Generate rivers going from highest point to sea level
4. Generate foliage

#### Generate Biome Distribution

1. Assign modifier biomes an index
2. Create terrain with default modifier biome
3. Randomly select a location for the number of extra biomes
4. Add extra biomes with a spacing of at least the minimum biome size
5. Create height map using the heights from the extra biomes
   1. A height should be selected between the minimum and maximum heights
   2. The result should look like a [3D surface plot](https://www.google.com/search?q=3d+surface+plot)
6. Assign modifier biomes based on the heights of the rest of the points
   1. The value from `0.0` to `1.0` should be assigned to a biome by means of a percentage
      1. If there are 5 modifier biomes, then `0.0` to `0.2` will be assigned to the modifier biome with index 0
   2. Any point below the sea level will be set to the ocean biome

#### Generate Height Points

1. Flatten all heights
2. Modify height points based on biome height modifiers
   1. Set to a random value between the biomes minimum and maximum height
   2. Smooth out all points using a [moving average](https://en.wikipedia.org/wiki/Moving_average)
3. Use [perlin noise](https://en.wikipedia.org/wiki/Perlin_noise) to add noise to the height values of the map

#### Generate Rivers

#### Generate Foliage

## Main World Generation

Villages, minerals and foot paths are needed. Here are the generation phases:

1. Generate village locations
2. Generate mineral locations
3. Generate foot paths

## Race Course Generation

The race track, stadium and spectators need generation. Here are the generation phases:

1. Generate course route
2. Generate stamina boosting nut locations
3. If the race is a national race, generate stadium
4. Generate spectator locations

## Village Generation

Shops, venues for signing up for national races, avion stalls and plots for homes and gardens need to be generated. Here are the generation phases:

1. Generate national race venue
2. Generate shop locations
3. Generate avion stall locations
4. Generate home and garden plots
