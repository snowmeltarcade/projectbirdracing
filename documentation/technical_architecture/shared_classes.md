# Shared Classes

- [Shared Classes](#shared-classes)
  - [Game Manager](#game-manager)
    - [Threading](#threading)
    - [Graphics Thread](#graphics-thread)
    - [Audio Thread](#audio-thread)
    - [Network Thread](#network-thread)
    - [Logic Thread](#logic-thread)
  - [Log Manager](#log-manager)
    - [Graphics Logging](#graphics-logging)
  - [Data Manager](#data-manager)
  - [File Manager](#file-manager)
  - [Graphics Manager](#graphics-manager)
  - [Audio Manager](#audio-manager)
  - [Input Manager](#input-manager)
  - [Window Manager](#window-manager)
  - [Network Manager](#network-manager)
  - [Resource Manager](#resource-manager)
    - [Resource Manager Lists](#resource-manager-lists)
  - [Memory Manager](#memory-manager)
  - [Script Manager](#script-manager)
  - [Scene Manager](#scene-manager)
  - [Scene](#scene)
  - [UI Manager](#ui-manager)
  - [UI Renderer](#ui-renderer)
  - [ECS](#ecs)
  - [Physics Simulator](#physics-simulator)
  - [World](#world)
    - [Loading World](#loading-world)
    - [Main World](#main-world)
    - [Race Track](#race-track)
    - [World Data](#world-data)
  - [Action Manager](#action-manager)
  - [World Renderer](#world-renderer)
  - [Diagnostics Manager](#diagnostics-manager)
  - [Time Manager](#time-manager)
  - [Memory Manager](#memory-manager-1)

As the major game components should be able to run in their own threads, communication between these components will happen using thread safe message queues. The `Game` class will decide which components run in which threads.

These classes are used by both the client and server.

## Game Manager

Holds instances of all main modules and manages the game loop.

There are 4 ways to exit a game manager:

* User input (for instance, the user pressing the `escape` key)
* A window close event
* A UI event (for instance, clicking an `exit` button)
* An error

### Threading

There will be four main threads - graphics, audio, network and logic.

At the end of each frame, the graphics and audio manager will wait for the game manager to indicate that a new frame is ready to begin. During this time, any needed shared data can be synchronized.

### Graphics Thread

The graphics manager will run on this thread. Needed data will be passed by means of thread safe queues.

### Audio Thread

The audio manager will run on this thread. Needed data will be passed by means of thread safe queues.

### Network Thread

The network manager will run on this thread. Needed data will be passed by means of thread safe queues.

Both packet sending and receiving will happen on this thread. Non-blocking APIs will be used to check for received packets, so we can continue sending  any packets in the queue.

### Logic Thread

The remaining managers, the scene and ECS will all run on this thread.

For the server, all the scenes will run on this thread. If it becomes a bottleneck, as the scenes and worlds are designed to run on their own threads, we can split the scenes up to run on other threads.

## Log Manager

Handles logging to an output stream. An interface to log to a file is also provided.

The following log levels are provided:

| Level | Description |
| --- | --- |
| `info` | For informational messages |
| `warning` | For messages that require attention, but are not considered an error |
| `error` | For recoverable errors that require attention |
| `fatal` | For unrecoverable errors that require a system halt. It is the responsibility of the caller to halt the system |

This will be thread safe. Logs can pass in an ID identifying the thread the message is associated with.

### Graphics Logging

Graphics logs will be logged to `./graphics.log.txt` in the directory of the executable.

## Data Manager

Loads data from the `data` folder using the [file manager](#file-manager). It will load the files in a format that it knows and will expose this data in a format agnostic way.

The default data format is `JSON`.

When loading a file from the `data` folder, the file extension is not required. The data manager will search for the file using file extensions of formats it knows about.

Data can be set into the data manager, overwriting any data loaded from a file. New data can also be set.

## File Manager

Handles the loading of files from multiple sources. Currently, the following sources are supported:

1) Local file system
2) A networked location

Other sources may be supported in the future, such as compressed archives or Git repositories.

A URI will be provided and a binary blob of the file data will be returned as a promise. This allows asynchronous file loading, which is very useful if a file is either very large or needs to be downloaded from a network location.

No cache functionality will currently be supported.

## Graphics Manager

Handles rendering all graphical elements.

See [here](./graphics.md) for more information.

## Audio Manager

The audio loop will be managed here.

## Input Manager

Keyboard, mouse, touch, joystick and game controller input will be managed here.

## Window Manager

Application windows will be managed here.

A separate console window will also be created to display any output from the standard streams.

A window will have a title and a size. If the window is full screen, the resolution will be the window size. If the window is not full screen, the window position will start centered in the screen.

The window loop will be handled by a `window` class. The window exit event will be passed to the game manager, which will then request the game manager to exit.

## Network Manager

TCP and UDP streams, as well as packet sending and receiving will be managed here.

Packets to send are added to a send queue. Received packets are stored on a receive queue. These queues are thread safe so other managers can access these queues.

See [here](./networking.md) for more information.

## Resource Manager

Loads and frees any needed resources, such as images, mesh, audio and world data.

To avoid loading the same resource more than once, each time a resource is requested, its usage count will increase. If it is not yet loaded, the resource will first be loaded. When a resource is freed, its usage count is decreased. If the usage count becomes zero, the resource is destroyed and any memory used is freed.

### Resource Manager Lists

Resource are detected by placing them in a settings file called `list.json`. When a resource manager loads, it searches for resources by means of this file. The format of this file is as follows:

```json
{
  "resources": [
    {
      "name": "name",
      "path": "path"
    }
  ]
}
```

`name` is the name of the resource. This name will be used to load the resource from the resource manager's `get()` function.

`path` is the path relative to the `list.json` file.

Note: the key must be `resources` for the resource manager to detect the resources.

## Memory Manager

Needed memory will be requested and freed through this manager. It will keep a log of how much memory is used.

To help with thread safety, each thread should have its own memory manager.

## Script Manager

Loads and executes scripts. These scripts will be used by the UI system and ECS.

## Scene Manager

Loads, destroys and runs the active scene. On the server, will hold all active scenes - the main world, villages, racetracks etc...

The client and server will register the scenes they need with the scene manager, along with a loading scene. The loading scene will be the initial scene that is loaded, and also the scene that is loaded when a new scene load is requested.

Scenes that are to be loaded are queued with the scene manager. Typically, the client will only queue one scene at a time. The server will queue all scenes that need running. When the needed scenes are queued, a request is made to load those scenes. Whilst those scenes are being loaded, the loading scene will first be loaded and then run. It will be unloaded when all of the queued scenes have been loaded. The queued scenes will then be run.

## Scene

Runs all aspects of the scene - the UI manager, the ECS, the physics simulator and the world.

## UI Manager

Loads and manages the UI. Handles the rendering of the UI.

## UI Renderer

Renders the UI.

## ECS

Updates and manages all entities in the world.

## Physics Simulator

Processes physics simulations in the world. On the server, this will manage simulations for entities that are shared between clients. On the client, this will manage simulations for local, non-shared entities that do not affect game play (such as particles, rocks etc...)

## World

Manages the mechanics and rules affecting the world itself.

There are three types of world - loading, world and race track.

### Loading World

Will only run on the client. Manages the loading of a new world. The server will have all worlds loaded, so there is no need for this world on the server.

### Main World

The world map. This includes the outside world and all villages.

### Race Track

All races will happen on a race track. This world will manage all race mechanics.

### World Data

The world data itself will be stored as a 2D grid of tiles. Each tile has a type. The client will interpret these types and will render accordingly.

All changes to the world data will be added to a ledger. This will be a list storing the latest types of all changed tiles. This entire ledger will be sent to all clients each time an entry is made. The client will parse this list and update the respective tile types accordingly.

Each time an entry is made, any previous entries into the ledger at that X/Y index should be updated with the new tile type. This will avoid duplicate entries in the ledger and will also avoid the ledger growing any bigger than the maximum size of the world itself.

The fields needed for each ledger entry are:

| Field | Type |
| --- | --- |
| Tile X Index | Integer |
| Tile Y Index | Integer |
| Tile Type | Tile Type |

## Action Manager

Converts input into actions that are acted on by entities. Input for actions can come from either the input manager or the network manager by means of data packets.

Entities will only act on actions, so this manager abstracts out the source of such actions.

On the server, all actions for NPCs will be generated by AI. These actions will be sent to the client, so they can update their local simulations. PCs on the server get their actions from the client. Those actions will then also be sent to the other clients, so they can update their local simulations.

## World Renderer

Renders the world and all entities within it.

## Diagnostics Manager

Manages system information, assists with debugging and offers profiling and sampling APIs.

## Time Manager

Manages in game time.

Server time is considered the 'current' time. Server time will be sent to clients every 100ms to help them keep in sync.

## Memory Manager

Exposes memory request and release APIs. Keeps track of how much memory is being used. Provides object pooling functionality.

Each thread will have its own memory manager.
