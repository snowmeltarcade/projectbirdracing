# World Generation

- [World Generation](#world-generation)
  - [Terrain Generation](#terrain-generation)
  - [Main World Generation](#main-world-generation)
  - [Race Course Generation](#race-course-generation)
  - [Village Generation](#village-generation)

All worlds have the same terrain generation algorithm. Specific worlds will have further refinements according to their types.

## Terrain Generation

The main phases of this algorithm are:

1. Generate biome distribution
2. Generate rivers going from highest point to sea level
3. Generate foliage

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
