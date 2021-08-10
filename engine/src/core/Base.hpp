#pragma once

#include "core/Logger.hpp"
// clang-format off
#include <glad/glad.h>
#include <GLFW/glfw3.h>
// clang-format on
#include <algorithm>
#include <functional>
#include <glm/glm.hpp>
#include <list>
#include <map>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#ifdef _WIN32
    #define BIRDY3D_PLATFORM_WINDOWS
#elif __APPLE__
    #include <TargetConditionals.h>
    #if TARGET_OS_MAC == 1
        #define BIRDY3D_PLATFORM_MACOS
        #error "MacOS is not supported"
    #else
        #error "Unknown Apple platform"
    #endif
#elif __linux__
    #define BIRDY3D_PLATFORM_LINUX
#else
    #error "Unknown platform"
#endif
