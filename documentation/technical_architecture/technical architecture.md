# Technical Architecture

- [Technical Architecture](#technical-architecture)
  - [Automation](#automation)
  - [Version Control](#version-control)
  - [Testing](#testing)
  - [Decoupling](#decoupling)
  - [Reduced Complexity](#reduced-complexity)
  - [Well Designed](#well-designed)
  - [Programming Language](#programming-language)
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

## Client

## Server

## Data Store
