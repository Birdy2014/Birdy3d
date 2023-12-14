# Birdy3d
A 3D game engine written in C++20.
It's main purpose is for me to learn C++ and OpenGL.

## Building
[![Build](https://github.com/Birdy2014/Birdy3d/actions/workflows/build.yml/badge.svg)](https://github.com/Birdy2014/Birdy3d/actions/workflows/build.yml)

Birdy3d supports both Linux and Windows.

Dependencies (the optional dependencies will be automatically downloaded if they are not found):
- CMake
- libgl
- wayland-protocols or Xorg (Linux)
- assimp (optional)
- glm (optional)
- freetype2 (optional)
- fmtlib (optional)

Compiling on Linux using GCC >= 12 or Clang >= 16 and Ninja:

```
cmake -B build -DCMAKE_BUILD_TYPE=Release -G Ninja
cmake --build build
```

Windows with Visual Studio 2019 (or newer):

```
cmake -B build -G 'Visual Studio 16 2019'
cmake --build build --config Release
```

The Executable can then be found under *build/out/bin*.
