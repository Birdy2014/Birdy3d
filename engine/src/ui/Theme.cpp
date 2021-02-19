#include "ui/Theme.hpp"

#include "core/Logger.hpp"

namespace Birdy3d {

    Theme::Theme(glm::vec4 color_fg, glm::vec4 color_bg, glm::vec4 color_border)
        : color_fg(color_fg)
        , color_bg(color_bg)
        , color_border(color_border) { }

    Theme::Theme(const std::string& color_fg, const std::string& color_bg, const std::string& color_border)
        : color_fg(parseColor(color_fg))
        , color_bg(parseColor(color_bg))
        , color_border(parseColor(color_border)) {
    }

    glm::vec4 Theme::parseColor(const std::string& colorString) {
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
            color[pos / colorLength] = nr / std::pow(16, colorLength);
        }
        return color;
    }

}
