# Shared Classes

- [Shared Classes](#shared-classes)
  - [Game](#game)
  - [Log Manager](#log-manager)
  - [Graphics Manager](#graphics-manager)
  - [Audio Manager](#audio-manager)
  - [Input Manager](#input-manager)
  - [Window Manager](#window-manager)
  - [Network Manager](#network-manager)
  - [Resource Manager](#resource-manager)
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

These classes are used by both the client and server.

## Game

Holds instances of all main modules and manages the game loop.

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

## Audio Manager

## Input Manager

## Window Manager

## Network Manager

## Resource Manager

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
