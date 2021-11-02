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
  - [Programming Language](#programming-language)
    - [Code Practices](#code-practices)
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
  - [Client](#client)
  - [Server](#server)
  - [Data Store](#data-store)

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

All merges to `main` will trigger a build and development release. A documentation build and release will also take place.

Pushes to all branches will have their code scanned using the `CodeQL` Github action.

### Releases

A nighty release will be built from the `main` branch. Only the most recent 7 day's of nightly releases will be kept. The nightly release will consist of a debug build, a release build and a release build with debug information.

Major releases will consist only of release builds and will be hosted using Github's release feature. All major releases will be tagged on the `main` branch. If a major release requires a patch, this patch will be tagged on the release branch and the patch will be cherry picked into `main` if required.

## Testing

Code will be developed with testing in mind. Although 100% code coverage will not be possible, as much as possible should be tested using unit tests and integration tests.

Tests will be run by both the developer and by the build pipelines.

[Catch2](https://github.com/catchorg/Catch2) will be used as the unit test framework.

## Decoupling

As much as possible, code will modularized. This will help with code sharing and testing.

The client and server will share as much code as possible by making use of these modules.

## Reduced Complexity

Code and processes will be kept as simple as possible.

## Well Designed

As much as possible, all high level designs will be documented. The code itself will be self describing.

## Programming Language

Primarily C++17 will be used. The aim is to use C++20 when it has better support in Clang. For now, C++20 concepts will also be used where possible.

The latest version of Clang will be used.

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

### Error Handling

As far as it possible, all functions should be marked `[[nodiscard]]`, as this avoids bugs due to unused variables. It also forces functions to be used as they are designed to. Any exceptions should be noted in a comment.

We will not throw any exceptions. If an exception is thrown by something, this will truly be an exceptional circumstance. To enforce this, all functions should be marked as `noexcept`.

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

[Doxygen](https://www.doxygen.nl/index.html) will be used to generate documentation from these comments. The resulting documentation will be hosted by Github Pages.

## APIs

All APIs will be abstracted out into their own modules to keep code decoupled, testable and to reduce complexity.

The following APIs will be used:

### Windowing

[SDL 2.0](https://www.libsdl.org/) is a cross platform, C++ API that works on all target devices. It handles window creation and destruction, full screen, screen resolutions and event loop management.

### Graphics

[SDL 2.0](https://www.libsdl.org/) is a cross platform, C++ API that works on all target devices. This will be used to setup the graphics device and handle window management.

[SDL Image 2.0](https://www.libsdl.org/projects/SDL_image/) is a cross platform, C++ API that works on all target devices. It handles the loading of various image formats.

Actual rendering will be done using [Vulkan](https://www.vulkan.org/) for Windows, Linux and Android and [MoltenVK](https://github.com/KhronosGroup/MoltenVK/) for Mac and iOS.

### Math

[GLM](https://github.com/g-truc/glm) will be used to provide tested and optimized mathematical functions and classes.

### Input

[SDL 2.0](https://www.libsdl.org/) is a cross platform, C++ API that works on all target devices. Input from the keyboard, mouse and touch screen is handled by the event loop. Joysticks and game controllers are handled by separate APIs.

### Audio

[SDL Mixer 2.0](https://www.libsdl.org/projects/SDL_mixer/) is a cross platform, C++ API that works on all target devices. It handles playing basic audio files.

### Network

[SDL Net 2.0](https://www.libsdl.org/projects/SDL_net/) is a cross platform, C++ API that works on all target devices. It handles TCP and UDP connections.

### Scripting

[v8](https://v8.dev/) is a cross platform scripting engine that uses JavaScript. The version is updated regularly, so the latest version will used when it is released.

## Client

See [here](client.md) for information about the client.

## Server

See [here](server.md) for information about the server.

## Data Store

Persistent data will be stored in a [SQLite](https://www.sqlite.org/index.html).
