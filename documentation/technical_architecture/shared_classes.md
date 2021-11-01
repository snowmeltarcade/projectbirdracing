# Shared Classes

- [Shared Classes](#shared-classes)
  - [Game](#game)
    - [Threading](#threading)
    - [Graphics Thread](#graphics-thread)
    - [Audio Thread](#audio-thread)
    - [Network Thread](#network-thread)
    - [Logic Thread](#logic-thread)
  - [Log Manager](#log-manager)
  - [Graphics Manager](#graphics-manager)
  - [Audio Manager](#audio-manager)
  - [Input Manager](#input-manager)
  - [Window Manager](#window-manager)
  - [Network Manager](#network-manager)
  - [Resource Manager](#resource-manager)
  - [Memory Manager](#memory-manager)
  - [Script Manager](#script-manager)
  - [Scene Manager](#scene-manager)
  - [Scene](#scene)
  - [UI Manager](#ui-manager)
  - [UI Renderer](#ui-renderer)
  - [ECS](#ecs)
  - [Physics Simulator](#physics-simulator)
  - [World](#world)
  - [Action Manager](#action-manager)
  - [World Renderer](#world-renderer)

As the major game components should be able to run in their own threads, communication between these components will happen using thread safe message queues. The `Game` class will decide which components run in which threads.

These classes are used by both the client and server.

## Game

Holds instances of all main modules and manages the game loop.

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

For the server, all the scenes will run on this thread. If it becomes a bottleneck, as the scenes and worlds are designed to run on their own threads anyway, we can split the scenes up to run on other threads.

## Log Manager

Handles logging to an output stream. An interface to log to a file is also provided.

The following log levels are provided:

| Level | Description |
| --- | --- |
| `info` | For informational messages |
| `warning` | For messages that require attention, but are not considered an error |
| `error` | For recoverable errors that require attention |
| `fatal` | For unrecoverable errors that require a system halt. It is the responsibility of the caller to halt the system |

## Graphics Manager

The rendering loop will be managed here.

## Audio Manager

The audio loop will be managed here.

## Input Manager

Keyboard, mouse, touch, joystick and game controller input will be managed here.

## Window Manager

Application windows will be managed here. A separate console window will also be created to display any output from the standard streams.

## Network Manager

TCP and UDP streams, as well as packet sending and receiving will be managed here.

Packets to send are added to a send queue. Received packets are stored on a receive queue. These queues are thread safe so other managers can access these queues.

## Resource Manager

Loads and frees any needed resources, such as images, mesh, audio and world data.

To avoid loading the same resource more than once, each time a resource is requested, its usage count will increase. If it is not yet loaded, the resource will first be loaded. When a resource is freed, its usage count is decreased. If the usage count becomes zero, the resource is destroyed and any memory used is freed.

## Memory Manager

## Script Manager

## Scene Manager

## Scene

## UI Manager

## UI Renderer

## ECS

## Physics Simulator

## World

## Action Manager

## World Renderer
