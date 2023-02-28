#pragma once

#include "core/Base.hpp"
#include "utils/serializer/Adapter.hpp"

namespace Birdy3d::utils {

    class Color {
    public:
        enum class Name {
            BLACK,
            RED,
            GREEN,
            YELLOW,
            BLUE,
            PURPLE,
            CYAN,
            WHITE,
            GRAY,
            LIGHT_RED,
            LIGHT_GREEN,
            LIGHT_YELLOW,
            LIGHT_BLUE,
            LIGHT_PURPLE,
            LIGHT_CYAN,
            LIGHT_WHITE,
            FG,
            BG,
            BORDER,
            BG_TITLE_BAR,
            BG_INPUT,
            BG_SELECTED,
            TEXT_HIGHLIGHT,
            OBJECT_SELECTION,
            COLLIDER_WIREFRAME,
            NONE,
        };

        static const Color NONE;
        static const Color WHITE;
        static const Color BLACK;

        glm::vec4 value;

        Color();
        Color(std::string const& color);
        Color(char const* color);
        Color(glm::vec4 color);
        operator glm::vec4() const;
        [[nodiscard]] std::string to_string() const;
        Color& operator=(std::string const& color);
        bool operator<(Color const& other) const;
        static glm::vec4 parse(std::string const& color_string);
        float r() { return value.r; }
        float g() { return value.g; }
        float b() { return value.b; }
        void r(float v) { value.r = v; }
        void g(float v) { value.g = v; }
        void b(float v) { value.b = v; }
    };

}

namespace Birdy3d::serializer {

    template <>
    Value adapter_save(utils::Color&);

    template <>
    void adapter_load(Value*, utils::Color&);

}

template <>
struct fmt::formatter<Birdy3d::utils::Color> {
    constexpr auto parse(format_parse_context& ctx) -> decltype(ctx.begin())
    {
        auto it = ctx.begin(), end = ctx.end();
        if (it != end && *it != '}')
            throw format_error("invalid format");
        return it;
    }

    template <typename FormatContext>
    auto format(Birdy3d::utils::Color const& color, FormatContext& ctx) -> decltype(ctx.out())
    {
        auto r = static_cast<int>(color.value.r * 255);
        auto g = static_cast<int>(color.value.g * 255);
        auto b = static_cast<int>(color.value.b * 255);
        return format_to(ctx.out(), "Color{{\033[38;2;{};{};{}m{}\033[0m}}", r, g, b, color.value);
    }
};
