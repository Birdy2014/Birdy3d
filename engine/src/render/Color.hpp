#pragma once

#include <glm/glm.hpp>
#include <string>

namespace Birdy3d {

    class Color {
    public:
        static Color WHITE;
        static Color BLACK;

        glm::vec4 value;

        Color();
        Color(const std::string& color);
        Color(const char* color);
        Color(glm::vec4 color);
        operator glm::vec4() const;
        Color& operator=(const std::string& color);
        bool operator<(const Color& other) const;
        static glm::vec4 parse(const std::string& colorString);
    };

}
