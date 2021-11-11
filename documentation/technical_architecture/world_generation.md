# World Generation

- [World Generation](#world-generation)
  - [Terrain Generation](#terrain-generation)
    - [Terrain Generation Inputs](#terrain-generation-inputs)
    - [Terrain Generation Algorithm](#terrain-generation-algorithm)
      - [Generate Biome Distribution](#generate-biome-distribution)
      - [Generate Height Points](#generate-height-points)
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
3. Generate foliage

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
   1. For each biome, generate a height map using [perlin noise](https://en.wikipedia.org/wiki/Perlin_noise)
      1. Use the biome's minimum and maximum height, number of octaves and redistribution values
         1. See [here](https://www.redblobgames.com/maps/terrain-from-noise/) for a good description of height map generation
      2. Multiple each octave's height map together to form the final height map
   2. Iterate over each point
      1. For that point's biome, set the point height to the height maps point
3. Smooth out all points using a [moving average](https://en.wikipedia.org/wiki/Moving_average)
4. Use [perlin noise](https://en.wikipedia.org/wiki/Perlin_noise) to add noise to the height values of the map

#### Generate Foliage

Use [Poisson Disk Sampling](http://devmag.org.za/2009/05/03/poisson-disk-sampling/) for foliage location generation.

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
