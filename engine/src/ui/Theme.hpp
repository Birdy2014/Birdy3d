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
        const unsigned int fontSize;

        Theme(Color color_fg, Color color_bg, Color color_border, unsigned int fontSize);

        // TODO: load theme from file
    };

}
