#pragma once

#include "render/Color.hpp"
#include <glm/glm.hpp>
#include <string>

namespace Birdy3d {

    class TextRenderer;

    class Theme {
    public:
        const Color color_fg;
        const Color color_bg;
        const Color color_border;
        const Color color_title_bar;
        const Color color_input_bg;
        const Color color_selected_bg;
        const Color color_text_highlight;
        const std::string font;
        const int font_size;
        const int line_height;

        Theme(Color color_fg, Color color_bg, Color color_border, Color color_title_bar, Color color_input_bg, Color color_selected_bg, Color color_text_highlight, std::string font, int font_size);
        TextRenderer& text_renderer();

        // TODO: load theme from file

    private:
        std::unique_ptr<TextRenderer> m_text_renderer;
    };

}
