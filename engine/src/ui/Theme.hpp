#pragma once

#include "core/Base.hpp"
#include "ui/TextRenderer.hpp"
#include "utils/Color.hpp"
#include "utils/serializer/Adapter.hpp"

namespace Birdy3d::ui {

    class TextRenderer;

    class Theme {
    public:
        Theme(std::string const& file_content);
        [[nodiscard]] TextRenderer& text_renderer() const;
        [[nodiscard]] utils::Color color(utils::Color::Name) const;
        [[nodiscard]] std::string font() const;
        [[nodiscard]] int font_size() const;
        [[nodiscard]] int line_height() const;
        void serialize(serializer::Adapter&);

    private:
        std::array<utils::Color, 16> m_termcolors;
        utils::Color m_color_fg;
        utils::Color m_color_bg;
        utils::Color m_color_border;
        utils::Color m_color_bg_title_bar;
        utils::Color m_color_bg_input;
        utils::Color m_color_bg_selected;
        utils::Color m_color_text_highlight;
        utils::Color m_color_object_selection;
        utils::Color m_color_collider_wireframe;

        std::string m_font;
        int m_font_size;
        int m_line_height;

        std::unique_ptr<TextRenderer> m_text_renderer;
    };

}
