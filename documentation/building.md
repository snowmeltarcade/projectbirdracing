# Building

- [Building](#building)
  - [Code](#code)
    - [CLion](#clion)
  - [Documentation](#documentation)

## Code

To build, run `build.py`:
```
python3 build.py
```

`build.py` takes the following flags:
* `-i` / `--install-dependencies`
    Install the library dependencies before build. Defaults to `false`.
* `-nb` / `--no-build`
    Skip the build step. Defaults to `false`.
* `-ni` / `--no-install`
    Skip the install step. Defaults to `false`.
* `-c` / `--cleanup`
    Deletes the build and install data/temp files after build. Defaults to `false`.
* `-d` / `--debug`
    Build in debug

After the build, the client, server and data will be copied to the `./src/install` directory. This directory has the following structure:

```
install
    + - _platform name_ // Windows/Darwin/Linux
        + - bin
            // SnowMeltArcade.ProjectBirdRacing.Project.exe on Windows and any other dependencies
```

Note: If `-c` is passed to `build.py`, the `./src/install` directory will be deleted after the build.

A zipped version of `./src/install` will be created in `./src/archives`. The resulting filename has the format:
```
SnowMeltArcade.ProjectBirdRacing-version-platform.zip
```

For instance:
```
SnowMeltArcade.ProjectBirdRacing-1.2.3-darwin.zip
```

### CLion

Open the project at the project root and then open the `CMakeLists.txt` file in the `./src` directory. CLion should then prompt you to load the project.

Set the CMake settings as follows:

> Toolchains -> Make -> /*{project_root}*/libraries/ninja/darwin/ninja

> CMake -> Toolchain -> *Set to the toolchain with Ninja as the make tool*

> CMake -> CMake options -> -GNinja -DCMAKE_CXX_COMPILER=/*{project_root}*/src/libraries/clang/darwin/bin/clang++ -DCMAKE_C_COMPILER=/*{project_root}*/src/libraries/clang/darwin/bin/clang

## Documentation

To build the documentation, run `build_documentation.py`:
```
python3 build_documentation.py
```

This will build the documentation in `/documentation/code/`.
