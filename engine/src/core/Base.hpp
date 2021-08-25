#pragma once

#include "core/Logger.hpp"
// clang-format off
#include <glad/glad.h>
#include <GLFW/glfw3.h>
// clang-format on
#include <algorithm>
#include <cereal/archives/json.hpp>
#include <cereal/types/memory.hpp>
#include <cereal/types/polymorphic.hpp>
#include <cereal/types/string.hpp>
#include <cereal/types/vector.hpp>
#include <functional>
#include <glm/glm.hpp>
#include <list>
#include <map>
#include <memory>
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

namespace glm {

    template <class Archive>
    void serialize(Archive& ar, glm::vec2& v) {
        ar(cereal::make_nvp("x", v.x), cereal::make_nvp("y", v.y));
    }

    template <class Archive>
    void serialize(Archive& ar, glm::vec3& v) {
        ar(cereal::make_nvp("x", v.x), cereal::make_nvp("y", v.y), cereal::make_nvp("z", v.z));
    }

    template <class Archive>
    void serialize(Archive& ar, glm::vec4& v) {
        ar(cereal::make_nvp("x", v.x), cereal::make_nvp("y", v.y), cereal::make_nvp("z", v.z), cereal::make_nvp("w", v.w));
    }

}
