#pragma once

#include <fmt/format.h>
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>
#include <iostream>

namespace Birdy3d::core {

    class Logger {
    public:
        template <typename... Args>
        static void debug(const std::string_view format_string, const Args&... args) {
            auto formatted = "DEBUG: " + fmt::vformat(format_string, fmt::make_format_args(args...));
            std::cout << formatted << '\n';
            print_console(formatted, Type::DEBUG);
        }

        template <typename... Args>
        static void warn(const std::string_view format_string, const Args&... args) {
            auto formatted = "WARNING: " + fmt::vformat(format_string, fmt::make_format_args(args...));
            std::cerr << formatted << '\n';
            print_console(formatted, Type::WARN);
        }

        template <typename... Args>
        static void error(const std::string_view format_string, const Args&... args) {
            auto formatted = "ERROR: " + fmt::vformat(format_string, fmt::make_format_args(args...));
            std::cerr << formatted << '\n';
            print_console(formatted, Type::ERROR);
        }

        template <typename... Args>
        static void critical(const std::string_view format_string, const Args&... args) {
            auto formatted = "CRITICAL: " + fmt::vformat(format_string, fmt::make_format_args(args...));
            std::cerr << formatted << '\n';
            std::abort();
        }

        template <typename T>
        static void assert_not_null(void* obj, const T& message) {
            if (obj == nullptr)
                Logger::critical("Assert not null failed: {}", message);
        }

    private:
        enum class Type {
            DEBUG,
            WARN,
            ERROR
        };

        static void print_console(const std::string&, Type);
    };

}

template <glm::length_t L, typename T>
struct fmt::formatter<glm::vec<L, T>> {
    constexpr auto parse(format_parse_context& ctx) -> decltype(ctx.begin()) {
        auto it = ctx.begin(), end = ctx.end();
        if (it != end && *it != '}')
            throw format_error("invalid format");
        return it;
    }

    template <typename FormatContext>
    auto format(const glm::vec<L, T>& v, FormatContext& ctx) -> decltype(ctx.out()) {
        return format_to(ctx.out(), "{}", glm::to_string(v));
    }
};

#define BIRDY3D_TODO ::Birdy3d::core::Logger::critical("Not implemented: {}", __PRETTY_FUNCTION__);
