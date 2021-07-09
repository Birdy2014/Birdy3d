#pragma once

#include "render/Color.hpp"
#include "ui/Utils.hpp"
#include <glm/glm.hpp>
#include <string>

namespace Birdy3d {

    class TextRenderer;

    class Theme {
    public:
        const Color color_fg;
        const Color color_bg;
        const Color color_border;
        const Color color_input_bg;
        const std::string font;
        const unsigned int fontSize;
        const unsigned int lineHeight;

        Theme(Color color_fg, Color color_bg, Color color_border, Color color_input_bg, std::string font, unsigned int fontSize);

        TextRenderer* text_renderer();

        // TODO: load theme from file
    };

}
