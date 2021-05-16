#pragma once

#include "ui/Utils.hpp"
#include <glm/glm.hpp>
#include <string>

namespace Birdy3d {

    class Theme {
    public:
        const Color color_fg;
        const Color color_bg;
        const Color color_border;
        const std::string font;
        const unsigned int fontSize;
        const unsigned int lineHeight;

        Theme(Color color_fg, Color color_bg, Color color_border, std::string font, unsigned int fontSize);

        // TODO: load theme from file
    };

}
