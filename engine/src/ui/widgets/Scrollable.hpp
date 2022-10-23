#pragma once

#include "ui/Widget.hpp"

#include "ui/Rectangle.hpp"

namespace Birdy3d::ui {

    class Scrollable : public Widget {
    public:
        Scrollable(is_widget_options auto options)
            : Widget(options) {
            m_scrollbar_vertical = add_filled_rectangle(0_px, Size(10_px, 100_pc), utils::Color::Name::NONE, Placement::TOP_RIGHT);
            m_scrollbar_horizontal = add_filled_rectangle(0_px, Size(100_pc, 10_px), utils::Color::Name::NONE, Placement::BOTTOM_LEFT);
            m_scrollbar_vertical->in_foreground = true;
            m_scrollbar_horizontal->in_foreground = true;
            m_padding = {
                .left = 0_px,
                .right = 10_px,
                .top = 0_px,
                .bottom = 10_px
            };
        }

        glm::ivec2 minimal_size() override;
        void arrange(glm::ivec2 pos, glm::ivec2 size) override;

    protected:
        bool m_horizontal_scroll_enabled = true;
        bool m_vertical_scroll_enabled = true;
        glm::ivec2 m_scroll_offset { 0 };
        glm::ivec2 m_content_size;
        glm::ivec2 m_max_scroll_offset { 0 };

        void draw() override;
        void on_scroll(ScrollEvent&) override;
        void on_click(ClickEvent&) override;
        void on_update() override;
        void on_resize(ResizeEvent&) override;

    private:
        Rectangle* m_scrollbar_vertical;
        Rectangle* m_scrollbar_horizontal;
        bool m_scrollbar_vertical_grabbed = false;
        bool m_scrollbar_horizontal_grabbed = false;

        void check_scroll_bounds();
    };

}
