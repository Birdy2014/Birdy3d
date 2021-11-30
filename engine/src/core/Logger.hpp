#pragma once

#include <glm/gtx/string_cast.hpp>
#include <iostream>
#include <sstream>

namespace Birdy3d::core {

    class Logger {
    public:
        template <typename... Args>
        static void debug(Args... args) {
            std::stringstream stream;
            stream << "DEBUG: ";
            print(stream, args...);
            std::cout << stream.str() << '\n';
            print_console(stream.str(), Type::DEBUG);
        }

        template <typename... Args>
        static void warn(Args... args) {
            std::stringstream stream;
            stream << "WARNING: ";
            print(stream, args...);
            std::cerr << stream.str() << '\n';
            print_console(stream.str(), Type::WARN);
        }

        template <typename... Args>
        static void error(Args... args) {
            std::stringstream stream;
            stream << "ERROR: ";
            print(stream, args...);
            std::cerr << stream.str() << '\n';
            print_console(stream.str(), Type::ERROR);
        }

        template <typename... Args>
        static void critical(Args... args) {
            std::cerr << "CRITICAL: ";
            print(std::cerr, args...);
            std::abort();
        }

        template <typename T>
        static void assert_not_null(void* obj, const T& message) {
            if (obj == nullptr)
                Logger::critical("Assert not null failed: ", message);
        }

    private:
        template <typename T>
        static void print(std::ostream& stream, const T& message) {
            if constexpr (std::is_same<T, glm::vec4>::value || std::is_same<T, glm::vec3>::value || std::is_same<T, glm::vec2>::value || std::is_same<T, glm::mat4>::value)
                stream << glm::to_string(message);
            else
                stream << message;
        }

        template <typename T, typename... Args>
        static void print(std::ostream& stream, const T& message, Args... args) {
            print(stream, message);
            print(stream, args...);
        }

        enum class Type {
            DEBUG,
            WARN,
            ERROR
        };

        static void print_console(const std::string&, Type);
    };

}

#define BIRDY3D_TODO ::Birdy3d::core::Logger::critical("Not implemented: ", __PRETTY_FUNCTION__);
