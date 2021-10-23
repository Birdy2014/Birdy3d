#pragma once

// clang-format off
#include <glad/glad.h>
#include <GLFW/glfw3.h>
// clang-format on
#include <algorithm>
#include <array>
#include <functional>
#include <glm/glm.hpp>
#include <list>
#include <map>
#include <memory>
#include <numbers>
#include <string>
#include <unordered_map>
#include <vector>

#if defined(_WIN32)
    #define BIRDY3D_PLATFORM_WINDOWS
#elif defined(__APPLE__)
    #include <TargetConditionals.h>
    #if TARGET_OS_MAC == 1
        #define BIRDY3D_PLATFORM_MACOS
        #error "MacOS is not supported"
    #else
        #error "Unknown Apple platform"
    #endif
#elif defined(__linux__)
    #define BIRDY3D_PLATFORM_LINUX
#else
    #error "Unknown platform"
#endif

#ifndef __PRETTY_FUNCTION__
    #define __PRETTY_FUNCTION__ __func__
#endif

#include "core/Logger.hpp"
