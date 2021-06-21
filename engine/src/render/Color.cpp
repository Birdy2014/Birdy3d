#include "render/Color.hpp"

#include "core/Logger.hpp"

namespace Birdy3d {

    Color::Color() {
        value = glm::vec4(0, 0, 0, 1);
    }

    Color::Color(const std::string& color) {
        value = parse(color);
    }

    Color::Color(const char* color) {
        value = parse(std::string(color));
    }

    Color::Color(glm::vec4 color) {
        value = color;
    }

    Color::operator glm::vec4() const {
        return value;
    }

    Color& Color::operator=(const std::string& color) {
        value = parse(color);
        return *this;
    }

    bool Color::operator<(const Color& other) const {
        float val = value.r * 255 + value.g * 255 * 16 + value.b * 255 * 16 * 16 + value.a * 255 * 16 * 16 * 16;
        float otherval = other.value.r * 255 + other.value.g * 255 * 16 + other.value.b * 255 * 16 * 16 + other.value.a * 255 * 16 * 16 * 16;
        return val < otherval;
    }

    glm::vec4 Color::parse(const std::string& colorString) {
        bool hasHash = false;
        if (colorString.at(0) == '#')
            hasHash = true;

        size_t colorLength;
        if (colorString.length() == 6 + hasHash || colorString.length() == 8 + hasHash) {
            colorLength = 2;
        } else if (colorString.length() == 3 + hasHash) {
            colorLength = 1;
        } else {
            Logger::warn("Invalid color: " + colorString);
            return glm::vec4(1);
        }

        glm::vec4 color(1);

        for (size_t pos = hasHash; pos < colorString.length(); pos += colorLength) {
            int nr = std::stoi(colorString.substr(pos, colorLength), nullptr, 16);
            color[pos / colorLength] = nr / (std::pow(16, colorLength) - 1);
        }
        return color;
    }

    Color Color::WHITE = Color("#FFFFFFFF");
    Color Color::BLACK = Color("#000000FF");

}
