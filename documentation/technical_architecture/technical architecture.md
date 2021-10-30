# Technical Architecture

- [Technical Architecture](#technical-architecture)
  - [Automation](#automation)
  - [Version Control](#version-control)
  - [Testing](#testing)
  - [Decoupling](#decoupling)
  - [Reduced Complexity](#reduced-complexity)
  - [Well Designed](#well-designed)
  - [Programming Language](#programming-language)
  - [APIs](#apis)
    - [Windowing](#windowing)
    - [Graphics](#graphics)
    - [Input](#input)
    - [Audio](#audio)
    - [Network](#network)
    - [Scripting](#scripting)
  - [Client](#client)
  - [Server](#server)
  - [Data Store](#data-store)

They key technical aims for this project are:

* Automation
* Version Control
* Testing
* Decoupling
* Reduced Complexity
* Well Designed

## Automation

Building, testing, installing and running will all be automated using Python scripts.

## Version Control

All source code, assets, resources, scripts, documentation and designs will be versioned using Git.

The above will all be stored in the [https://github.com/snowmeltarcade/projectbirdracing](https://github.com/snowmeltarcade/projectbirdracing) repository.

Assets used in the project will be stored in the [https://github.com/snowmeltarcade/projectbirdracing-assets](https://github.com/snowmeltarcade/projectbirdracing-assets) repository.

Development files for assets used in the project will be stored in [https://github.com/snowmeltarcade/projectbirdracing-assets-dev](https://github.com/snowmeltarcade/projectbirdracing-assets-dev).

## Testing

Code will be developed with testing in mind. Although 100% code coverage will not be possible, as much as possible should be tested using unit tests and integration tests.

Tests will be run by both the developer and by the build pipelines.

## Decoupling

As much as possible, code will modularized. This will help with code sharing and testing.

The client and server will share as much code as possible by making use of these modules.

## Reduced Complexity

Code and processes will be kept as simple as possible.

## Well Designed

As much as possible, all high level designs will be documented. The code itself will be self describing.

## Programming Language

C++17 will be used. The latest version of Clang will be used.

The code must compile on Windows, Mac, Linux, iOS and Android. Platform specific code must be kept to a minimum. Cross platform APIs and frameworks will be used. As much as possible, STL should be used.

## APIs

All APIs will be abstracted out into their own modules to keep code decoupled, testable and to reduce complexity.

The following APIs will be used:

### Windowing

[SDL 2.0](https://www.libsdl.org/) is a cross platform, C++ API that works on all target devices. It handles window creation and destruction, full screen, screen resolutions and event loop management.

### Graphics

[SDL 2.0](https://www.libsdl.org/) is a cross platform, C++ API that works on all target devices. This will be used to setup the graphics device and handle window management.

[SDL Image 2.0](https://www.libsdl.org/projects/SDL_image/) is a cross platform, C++ API that works on all target devices. It handles the loading of various image formats.

Actual rendering will be done using [OpenGL 4.6](https://www.opengl.org/) for Windows, Mac and Linux and [OpenGL ES 3.0](https://www.khronos.org/api/opengles) for iOS and Android.

### Input

[SDL 2.0](https://www.libsdl.org/) is a cross platform, C++ API that works on all target devices. Input from the keyboard, mouse and touch screen is handled by the event loop. Joysticks and game controllers are handled by separate APIs.

### Audio

[SDL Mixer 2.0](https://www.libsdl.org/projects/SDL_mixer/) is a cross platform, C++ API that works on all target devices. It handles playing basic audio files.

### Network

[SDL Net 2.0](https://www.libsdl.org/projects/SDL_net/) is a cross platform, C++ API that works on all target devices. It handles TCP and UDP connections.

### Scripting

[v8](https://v8.dev/) is a cross platform scripting engine that uses JavaScript. The version is updated regularly, so the latest version will used when it is released.

## Client

## Server

## Data Store
