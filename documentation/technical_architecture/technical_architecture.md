# Technical Architecture

- [Technical Architecture](#technical-architecture)
  - [Automation](#automation)
  - [Version Control](#version-control)
    - [Branches](#branches)
    - [Pushes and Merges](#pushes-and-merges)
    - [Build Automation](#build-automation)
    - [Releases](#releases)
  - [Testing](#testing)
  - [Decoupling](#decoupling)
  - [Reduced Complexity](#reduced-complexity)
  - [Well Designed](#well-designed)
  - [Versioning](#versioning)
    - [Comparing Versions](#comparing-versions)
  - [Naming](#naming)
  - [Programming](#programming)
    - [Code Practices](#code-practices)
    - [Namespaces](#namespaces)
    - [Error Handling](#error-handling)
    - [Compiling](#compiling)
    - [Profiling and Optimization](#profiling-and-optimization)
    - [Static Analysis](#static-analysis)
    - [Comments and Documentation](#comments-and-documentation)
  - [APIs](#apis)
    - [Windowing](#windowing)
    - [Graphics](#graphics)
    - [Math](#math)
    - [Input](#input)
    - [Audio](#audio)
    - [Network](#network)
    - [Scripting](#scripting)
    - [JSON](#json)
    - [Compression/Decompression](#compressiondecompression)
  - [Client](#client)
  - [Server](#server)
  - [Engine](#engine)
  - [Data Store](#data-store)
  - [Game Logic](#game-logic)
    - [Avions](#avions)
    - [Mining](#mining)
    - [Farming](#farming)
    - [Breeding](#breeding)
    - [World Generation](#world-generation)

They key technical aims for this project are:

- Automation
- Version Control
- Testing
- Decoupling
- Reduced Complexity
- Well Designed

## Automation

Building, testing, installing and running will all be automated using Python scripts.

## Version Control

All source code, assets, resources, scripts, documentation and designs will be versioned using Git.

The above will all be stored in the [https://github.com/snowmeltarcade/projectbirdracing](https://github.com/snowmeltarcade/projectbirdracing) repository.

Assets used in the project will be stored in the [https://github.com/snowmeltarcade/projectbirdracing-assets](https://github.com/snowmeltarcade/projectbirdracing-assets) repository.

Development files for assets used in the project will be stored in [https://github.com/snowmeltarcade/projectbirdracing-assets-dev](https://github.com/snowmeltarcade/projectbirdracing-assets-dev).

### Branches

The main working branch will be called `main`. No development should happen on `main`, but on separate development branches prefixed with `dev-`. All releases will have their own branch, labeled `release-x.y.z`.

General testing, miscellaneous work etc... that needs to be version controlled should have a branch prefixed with `test-`. These should be deleted when they are no longer needed, although can be kept if deemed useful.

### Pushes and Merges

All merges to `main` must be linked to a work item and submitted via a pull request.

### Build Automation

Github actions will be used for build automation.

All merges to `main` will trigger a build and development release. A documentation build and release for botht he game code and scripting API will also take place.

Pushes to all branches will have their code scanned using the `CodeQL` Github action.

### Releases

A nighty release will be built from the `main` branch. Only the most recent 7 day's of nightly releases will be kept. The nightly release will consist of a debug build, a release build and a release build with debug information.

Major releases will consist only of release builds and will be hosted using Github's release feature. All major releases will be tagged on the `main` branch. If a major release requires a patch, this patch will be tagged on the release branch and the patch will be cherry picked into `main` if required.

## Testing

Code will be developed with testing in mind. Although 100% code coverage will not be possible, as much as possible should be tested using unit tests and integration tests.

Only public facing APIs (those in the `public` part of a class) require explicit unit tests. `private` and `protected` functions should be tested as part of testing the `public` functions.

Tests will be run by both the developer and by the build pipelines.

[Catch2](https://github.com/catchorg/Catch2) will be used as the unit test framework.

## Decoupling

As much as possible, code will modularized. This will help with code sharing and testing.

The client and server will share as much code as possible by making use of these modules.

## Reduced Complexity

Code and processes will be kept as simple as possible.

## Well Designed

As much as possible, all high level designs will be documented. The code itself will be self describing.

## Versioning

The server, client and shared libraries will follow the same versioning scheme:

> major.minor.build

Any time a major feature has been added, the `major` component must increase.

Bug fixes will increase the `minor` component.

For each build, the `build` number must increase.

Version numbers do not need to be consistent across the server, client or shared libraries.

### Comparing Versions

Comparing versions is defined as follows:

> `major` > `minor.build`

> `minor` > `build`

So, `2.0.0` is greater than `1.99.1234`, `3.4.5` is greater than `3.3.99` and `1.2.3` is greater than `1.2.2`.

## Naming

Projects will have the following naming scheme:

`organization.solution.project`

This solution's projects will have the following names:

| Name | Description |
| --- | --- |
| `SnowMeltArcade.ProjectBirdRacing.Client` | The client project |
| `SnowMeltArcade.ProjectBirdRacing.Server` | The server project |
| `SnowMeltArcade.ProjectBirdRacing.Shared` | The shared libraries project |

## Programming

Source code will be stored in the `/src` directory.

Primarily C++17 will be used. The aim is to use C++20 when it has better support in Clang. For now, C++20 concepts will also be used where possible.

The latest version of Clang will be used.

Clang format will be used for code formatting.

The code must compile on Windows, Mac, Linux, iOS and Android. Platform specific code must be kept to a minimum. Cross platform APIs and frameworks will be used. As much as possible, STL should be used.

[CMake](https://cmake.org/) will be used to manage projects and [Ninja](https://ninja-build.org/) will be used as the build system.

All code will be 64-bit.

### Code Practices

Where possible C++'s RAII will be used to create and destroy objects, using constructors and destructors, instead of using `init` and `shutdown` functions.

When declaring variables, specify the exact type you want - `uint32_t` instead of `int`, for example. When assigning from a function return value, use `auto` to allow the compiler to do this work for you.

Hide as much information as is reasonable. For instance, do:

```cpp
using IdType = uint32_t;
IdType _id = new_id();
```

instead of:

```cpp
uint32_t _id = ++_global_ids;
```

Avoid undefined behavior, as this can introduce subtle bugs when running on a different platform of upgrading a compiler version.

### Namespaces

The root namespace will be `pbr` - as in `Project Bird Racing`.

The client will have the root namespace: `pbr::client`.

The server will have the root namespace: `pbr::server`.

The shared library will have the root namespace: `pbr::shared`.

All project code, except the `main()` functions and the memory module's `operator new` and `operator delete` definitions, should be in one of the above respective namespaces. 

### Error Handling

As far as is possible, all functions should be marked `[[nodiscard]]`, as this avoids bugs due to unused variables. It also forces functions to be used as they are designed to. Any exceptions should be noted in a comment.

For the most part, we will not throw any exceptions. If an exception is thrown by something, this will truly be an exceptional circumstance. To enforce this, all functions should be marked as `noexcept`.

As constructors cannot return error values, they will check their arguments using an `assert`. If an error is encountered, an exception should be thrown, as an object cannot be used if construction has failed.

Destructors wil be used to shut down managers. If an error occurs, a message should be written to the log and the next step in shut down should be executed.

Functions that must indicate  error or success should return a boolean value, returning `true` for success and `false` to indicate an error. This will allow the system to halt on an error gracefully. If the error is unrecoverable, an `assert` must 
be used to halt the system. Before returning `false` or calling `assert`, the error must be written to the log if possible.

Error codes must be used and checked when performing `std::filesystem` calls.

### Compiling

Warnings as errors must be enabled and all warnings must be enabled. The following flags should be used:

> -Wall -Wextra -pedantic -Werror -pedantic-errors -Wshadow -Wstrict-aliasing

Where possible, debug builds must be built with ASAN enabled, using this flag:

> -fsanitize=address

See [here](https://clang.llvm.org/docs/AddressSanitizer.html) for more details.

### Profiling and Optimization

Premature optimization should be avoided wherever possible.

Good code practices should be used at all times, such as relying on [RVO](https://en.cppreference.com/w/cpp/language/copy_elision), passing by `const &`, using `std::vector<>` over `std::list<>` etc... Favor moves over copies.

`constexpr` must be used where possible to perform logic during compile time instead of runtime.

[Optick](https://github.com/bombomby/optick) will be used to provide in built profiling and sampling functionality.

### Static Analysis

As there are few free static analysis tools, use the tool that comes with the IDE you use.

### Comments and Documentation

All classes and public interfaces must be commented, stating their purpose, inputs and outputs.

[Doxygen](https://www.doxygen.nl/index.html) will be used to generate documentation from these comments. The resulting documentation will be hosted by Github Pages. The repository [https://github.com/snowmeltarcade/projectbirdracingdocumentation](https://github.com/snowmeltarcade/projectbirdracingdocumentation) will be used to host the generated code documentation. The code documentation will be generated each time a push is made to the `main` branch.

## APIs

All APIs will be abstracted out into their own modules to keep code decoupled, testable and to reduce complexity.

The following APIs will be used:

### Windowing

[SDL 2.0](https://www.libsdl.org/) is a cross platform, C++ API that works on all target devices. It handles window creation and destruction, full screen, screen resolutions and event loop management.

### Graphics

[SDL 2.0](https://www.libsdl.org/) is a cross platform, C++ API that works on all target devices. This will be used to setup the graphics device and handle window management.

[SDL Image 2.0](https://www.libsdl.org/projects/SDL_image/) is a cross platform, C++ API that works on all target devices. It handles the loading of various image formats.

Actual rendering will be done using one of two APIs:

- [Vulkan](https://www.vulkan.org/) for Windows, Linux and Android and [MoltenVK](https://github.com/KhronosGroup/MoltenVK/) for Mac and iOS.
- [OpenGL](https://www.opengl.org/) 4.6 for Windows, Linux, Android and iOS.

Vulkan graphics based memory management will be handled using the [Vulkan Memory Allocator](https://github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator).

[GLSL](https://www.khronos.org/opengl/wiki/OpenGL_Shading_Language#:~:text=The%20OpenGL%20Shading%20Language%20(GLSL,is%20a%20C%2Dstyle%20language.&text=The%20current%20version%20of%20GLSL%20is%204.60.) will be used as the shader language.

See [here](graphics.md) for more information.

### Math

[GLM](https://github.com/g-truc/glm) will be used to provide tested and optimized mathematical functions and classes.

### Input

[SDL 2.0](https://www.libsdl.org/) is a cross platform, C++ API that works on all target devices. Input from the keyboard, mouse and touch screen is handled by the event loop. Joysticks and game controllers are handled by separate APIs.

### Audio

[SDL Mixer 2.0](https://www.libsdl.org/projects/SDL_mixer/) is a cross platform, C++ API that works on all target devices. It handles playing basic audio files.

### Network

[SDL Net 2.0](https://www.libsdl.org/projects/SDL_net/) is a cross platform, C++ API that works on all target devices. It handles TCP and UDP connections.

### Scripting

[TypeScript](https://www.typescriptlang.org/) will be the scripting language. It will be compiled down to JavaScript.

The scripting engine will be [v8](https://v8.dev/), which is a cross platform scripting engine that uses JavaScript. The version is updated regularly, so the latest version will used when it is released.

Scripts will be compiled during game startup and then saved to a cache directory. Scripts will only be compiled if there is not cached compiled version or if the cached version is out of date. To do this, the [tsc](https://www.typescriptlang.org/docs/handbook/compiler-options.html) compiler will need to be shipped with the client and server.

APIs and classes will be defined in TypeScript [d.ts](https://www.typescriptlang.org/docs/handbook/declaration-files/introduction.html) files.

Documentation for the scripts will be generated from the [d.ts](https://www.typescriptlang.org/docs/handbook/declaration-files/introduction.html) files using [Type Doc](https://typedoc.org/).

### JSON

[Nlohmann JSON](https://github.com/nlohmann/json) will be used for reading/writing JSON.

### Compression/Decompression

[LZ4](https://en.wikipedia.org/wiki/LZ4_(compression_algorithm)) will be used.

## Client

See [here](client.md) for information about the client.

## Server

See [here](server.md) for information about the server.

## Engine

See [here](engine.md) for information about the engine.

## Data Store

Persistent data will be stored in a [SQLite](https://www.sqlite.org/index.html).

## Game Logic

As far as is possible, scripts will be used to manage game logic.

### Avions

All avions will have scripts to manage how their actions influence their attributes. These scripts will also manage time based effects, such as age and health.

### Mining

### Farming

### Breeding

### World Generation

See [here](world_generation.md) for more information.
