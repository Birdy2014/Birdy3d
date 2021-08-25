#pragma once

#include "core/Base.hpp"

namespace Birdy3d {

    class Color {
    public:
        static Color NONE;
        static Color WHITE;
        static Color BLACK;

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

        template <class Archive>
        std::string save_minimal(Archive const&) const {
            return to_string();
        }

        template <class Archive>
        void load_minimal(Archive const&, std::string const& str) {
            value = parse(str);
        }
    };

}
