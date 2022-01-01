# Engine

A specific engine needs to be constructed to run the game. The engine is fully focused on this game, and not to support any other applications.

## Game Loop

There are multiple game loops on different threads. Each loop needs synchronizing to enable data transfer, such as submitting renderable entities to the graphics manager.

Here is a diagram showing how each game loop is synchronized:

![Game Loop Synchronization](images/game_loops.drawio.png)
