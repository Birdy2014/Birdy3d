#pragma once

#include "core/Base.hpp"
#include "ui/TextRenderer.hpp"
#include "utils/Color.hpp"
#include "utils/serializer/Adapter.hpp"

namespace Birdy3d {

    class TextRenderer;

    class Theme {
    public:
        Theme(const std::string& path);
        TextRenderer& text_renderer();
        Color color(Color::Name) const;
        std::string font() const;
        int font_size() const;
        int line_height() const;
        void serialize(serializer::Adapter&);

    private:
        std::array<Color, 16> m_termcolors;
        Color m_color_fg;
        Color m_color_bg;
        Color m_color_border;
        Color m_color_bg_title_bar;
        Color m_color_bg_input;
        Color m_color_bg_selected;
        Color m_color_text_highlight;

        std::string m_font;
        int m_font_size;
        int m_line_height;

        std::unique_ptr<TextRenderer> m_text_renderer;
    };

}
