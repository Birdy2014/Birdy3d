#include "ui/Theme.hpp"

#include "core/Logger.hpp"
#include "core/RessourceManager.hpp"

namespace Birdy3d {

    Theme::Theme(Color color_fg, Color color_bg, Color color_border, Color color_input_bg, std::string font, unsigned int fontSize)
        : color_fg(color_fg)
        , color_bg(color_bg)
        , color_border(color_border)
        , color_input_bg(color_input_bg)
        , font(font)
        , fontSize(fontSize)
        , lineHeight(fontSize * 1.1) { }

    TextRenderer* Theme::text_renderer() {
        return RessourceManager::getTextRenderer(font).get();
    }
}
