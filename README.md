# Birdy3d
A 3D game engine written in C++.
Its main purpose is for me to learn C++ and OpenGL.

## Building
Dependencies (the optional dependencies will be automatically downloaded if they are not found):
- CMake
- libgl
- wayland-protocols or Xorg
- assimp (optional)
- glm (optional)
- freetype2 (optional)
- fmtlib (optional)

Compiling on Linux using GCC >= 12 or Clang >= 16 and Ninja:

```
cmake -B build -DCMAKE_BUILD_TYPE=Release -G Ninja
cmake --build build
```

The Executable can then be found under *build/out/bin*.
