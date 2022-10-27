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
