#include "ui/Theme.hpp"

#include "core/Logger.hpp"
#include "core/RessourceManager.hpp"

namespace Birdy3d {

    Theme::Theme(Color color_fg, Color color_bg, Color color_border, Color color_title_bar, Color color_input_bg, Color color_selected_bg, Color color_text_highlight, std::string font, unsigned int font_size)
        : color_fg(color_fg)
        , color_bg(color_bg)
        , color_border(color_border)
        , color_title_bar(color_title_bar)
        , color_input_bg(color_input_bg)
        , color_selected_bg(color_selected_bg)
        , color_text_highlight(color_text_highlight)
        , font(font)
        , font_size(font_size)
        , line_height(font_size * 1.1) { }

    TextRenderer* Theme::text_renderer() {
        return RessourceManager::getTextRenderer(font).get();
    }
}
