#include "core/Logger.hpp"

#include "ui/console/Console.hpp"
#include <glm/detail/qualifier.hpp>
#include <glm/gtx/string_cast.hpp>

namespace Birdy3d::core {

    void Logger::print_console(const std::string& text, Type type) {
        utils::Color::Name color = [&type] {
            switch (type) {
            case Type::DEBUG:
                return utils::Color::Name::NONE;
            case Type::WARN:
                return utils::Color::Name::YELLOW;
            case Type::ERROR:
                return utils::Color::Name::RED;
            }
            return utils::Color::Name::NONE;
        }();
        ui::Console::println(text, color);
    }

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
