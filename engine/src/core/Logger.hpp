#pragma once

#include <glm/gtx/string_cast.hpp>
#include <iostream>

namespace Birdy3d {

    class Logger {
    public:
        template <typename... Args>
        static void debug(Args... args) {
            std::cout << "DEBUG: ";
            print(true, std::cout, args...);
        }

        template <typename... Args>
        static void warn(Args... args) {
            std::cerr << "WARNING: ";
            print(true, std::cerr, args...);
        }

        template <typename... Args>
        static void error(Args... args) {
            std::cerr << "ERROR: ";
            print(true, std::cerr, args...);
            std::abort();
        }

        template <typename T>
        static void assertNotNull(void* obj, const T& message) {
            if (obj == nullptr)
                Logger::error("Assert not null failed: ", message);
        }

    private:
        template <typename T>
        static void print(bool lineend, std::ostream& stream, const T& message) {
            if constexpr (std::is_same<T, glm::vec4>::value || std::is_same<T, glm::vec3>::value || std::is_same<T, glm::vec2>::value || std::is_same<T, glm::mat4>::value)
                stream << glm::to_string(message);
            else
                stream << message;

            if (lineend)
                stream << std::endl;
        }

        template <typename T, typename... Args>
        static void print(bool, std::ostream& stream, const T& message, Args... args) {
            print(false, stream, message);
            print(true, stream, args...);
        }
    };

}

#define BIRDY3D_TODO Logger::error("Not implemented: ", __PRETTY_FUNCTION__);
