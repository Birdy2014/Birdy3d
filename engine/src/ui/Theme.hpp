#pragma once

#include <glm/glm.hpp>

namespace Birdy3d {

    class Theme {
    public:
        const glm::vec4 color_fg;
        const glm::vec4 color_bg;
        const glm::vec4 color_border;

        Theme(glm::vec4 color_fg, glm::vec4 color_bg, glm::vec4 color_border)
            : color_fg(color_fg)
            , color_bg(color_bg)
            , color_border(color_border) { }

        // TODO: parse color strings
        // TODO: load theme from file
    };

}
