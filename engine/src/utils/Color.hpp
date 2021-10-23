#pragma once

#include "core/Base.hpp"
#include "utils/serializer/Adapter.hpp"

namespace Birdy3d {

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
        };

        static const Color NONE;
        static const Color WHITE;
        static const Color BLACK;

        glm::vec4 value;

        Color();
        Color(const std::string& color);
        Color(const char* color);
        Color(glm::vec4 color);
        operator glm::vec4() const;
        std::string to_string() const;
        Color& operator=(const std::string& color);
        bool operator<(const Color& other) const;
        static glm::vec4 parse(const std::string& colorString);
        float r() { return value.r; }
        float g() { return value.g; }
        float b() { return value.b; }
        void r(float v) { value.r = v; }
        void g(float v) { value.g = v; }
        void b(float v) { value.b = v; }
    };

    namespace serializer {

        template <>
        std::unique_ptr<Value> adapter_save(Color&);

        template <>
        void adapter_load(Value*, Color&);

    }

}
