#include "render/Color.hpp"

#include "core/Logger.hpp"
#include <iomanip>

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

    std::string Color::to_string() const {
        std::stringstream stream;
        stream << '#' << std::setfill('0') << std::hex;
        stream << std::setw(2) << std::clamp((int)(value.r * 255), 0, 255);
        stream << std::setw(2) << std::clamp((int)(value.g * 255), 0, 255);
        stream << std::setw(2) << std::clamp((int)(value.b * 255), 0, 255);
        if ((int)(value.a * 255) > 0)
            stream << std::clamp((int)(value.a * 255), 0, 255);
        return stream.str();
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
        bool hasHash = colorString.at(0) == '#';

        int colorLength;
        if ((int)colorString.length() == 6 + hasHash || (int)colorString.length() == 8 + hasHash) {
            colorLength = 2;
        } else if ((int)colorString.length() == 3 + hasHash) {
            colorLength = 1;
        } else {
            Logger::warn("Invalid color: ", colorString);
            return glm::vec4(1);
        }

        glm::vec4 color(1);

        for (size_t pos = hasHash; pos < colorString.length(); pos += colorLength) {
            int nr = std::stoi(colorString.substr(pos, colorLength), nullptr, 16);
            color[pos / colorLength] = nr / (std::pow(16, colorLength) - 1);
        }
        return color;
    }

    Color Color::NONE = Color("#00000000");
    Color Color::WHITE = Color("#FFFFFFFF");
    Color Color::BLACK = Color("#000000FF");

}
