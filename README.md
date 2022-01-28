# Birdy3d
A 3D game engine written in C++20.
It's main purpose is for me to learn C++ and OpenGL.

This documentation will be expanded in the future.

## Building
[![Build](https://github.com/Birdy2014/Birdy3d/actions/workflows/build.yml/badge.svg)](https://github.com/Birdy2014/Birdy3d/actions/workflows/build.yml)

Birdy3d supports both Linux and Windows.

On Linux you need the following dependencies:
- GCC >= 10 or Clang >= 10
- CMake
- Make
- Xorg (Wayland is not supported yet)
- assimp (optional)
- glm (optional)
- freetype2 (optional)

```
cmake -B build -DCMAKE_BUILD_TYPE=Release -G Make
cmake --build build -- -j4
```

On Windows, you need:
- Visual Studio 2019 (or newer)
- CMake
- assimp (optional)
- glm (optional)
- freetype2 (optional)

```
cmake -B build -G 'Visual Studio 16 2019'
cmake --build build --config Release
```

The Executable can then be found under *build/out/bin*.
