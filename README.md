# Birdy3d
A 3D game engine written in C++20.
It's main purpose is for me to learn C++ and OpenGL.

This documentation will be expanded in the future.

## Building
[![Build](https://github.com/Birdy2014/Birdy3d/actions/workflows/build.yml/badge.svg)](https://github.com/Birdy2014/Birdy3d/actions/workflows/build.yml)

Birdy3d supports both Linux and Windows.

Dependencies:
- CMake
- assimp
- glm
- freetype2

Compiling on Linux using GCC >= 10 or Clang >= 10 and Make:

```
cmake -B build -DCMAKE_BUILD_TYPE=Release -G Make
cmake --build build -- -j4
```

Windows with Visual Studio 2019 (or newer):

```
cmake -B build -G 'Visual Studio 16 2019'
cmake --build build --config Release
```

The Executable can then be found under *build/out/bin*.
