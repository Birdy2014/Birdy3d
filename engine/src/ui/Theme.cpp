#include "ui/Theme.hpp"

#include "core/Logger.hpp"
#include "core/ResourceManager.hpp"
#include "utils/serializer/Serializer.hpp"
#include <fstream>

namespace Birdy3d {

    Theme::Theme(const std::string& file_content) {
        serializer::Serializer::deserialize(file_content, *this);
        m_text_renderer = std::make_unique<TextRenderer>(*this);
    }

    TextRenderer& Theme::text_renderer() {
        return *m_text_renderer.get();
    }

    Color Theme::color(Color::Name n) const {
        if ((int)n < 16)
            return m_termcolors[(int)n];
        switch (n) {
        case Color::Name::FG:
            return m_color_fg;
        case Color::Name::BG:
            return m_color_bg;
        case Color::Name::BORDER:
            return m_color_border;
        case Color::Name::BG_TITLE_BAR:
            return m_color_bg_title_bar;
        case Color::Name::BG_INPUT:
            return m_color_bg_input;
        case Color::Name::BG_SELECTED:
            return m_color_bg_selected;
        case Color::Name::TEXT_HIGHLIGHT:
            return m_color_text_highlight;
        default:
            return Color::NONE;
        }
    }

    std::string Theme::font() const {
        return m_font;
    }

    int Theme::font_size() const {
        return m_font_size;
    }

    int Theme::line_height() const {
        return m_line_height;
    }

    void Theme::serialize(serializer::Adapter& adapter) {
        adapter("termcolors", m_termcolors);
        adapter("color_fg", m_color_fg);
        adapter("color_bg", m_color_bg);
        adapter("color_border", m_color_border);
        adapter("color_bg_title_bar", m_color_bg_title_bar);
        adapter("color_bg_input", m_color_bg_input);
        adapter("color_bg_selected", m_color_bg_selected);
        adapter("color_text_highlight", m_color_text_highlight);
        adapter("font", m_font);
        adapter("font_size", m_font_size);
        adapter("line_height", m_line_height);
    }

}
