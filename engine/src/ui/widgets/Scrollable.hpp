#pragma once

#include "ui/Widget.hpp"

namespace Birdy3d::ui {

    // FIXME: Make scrollbars own widgets, so that they work in foreground. That also requires moving them
    // into foreground automatically in case someone calls to_foreground on any other widget.

    class Scrollable : public Widget {
    public:
        Scrollable(is_widget_options auto options)
            : Widget(options)
        {
            m_padding = {
                .left = 0_px,
                .right = 10_px,
                .top = 0_px,
                .bottom = 10_px};
        }

        glm::ivec2 minimal_size() override;
        virtual void do_layout(Rect const&) override;

    protected:
        bool m_horizontal_scroll_enabled = true;
        bool m_vertical_scroll_enabled = true;
        glm::ivec2 m_scroll_offset{0};
        glm::ivec2 m_max_scroll_offset{0};

        void draw() override;
        void on_scroll(ScrollEvent&) override;
        void on_click(ClickEvent&) override;
        void on_update() override;
        void on_resize(ResizeEvent&) override;
        virtual glm::ivec2 content_size() = 0;

    private:
        Rect m_scrollbar_vertical_rect;
        Rect m_scrollbar_horizontal_rect;
        bool m_scrollbar_vertical_visible;
        bool m_scrollbar_horizontal_visible;
        bool m_scrollbar_vertical_grabbed = false;
        bool m_scrollbar_horizontal_grabbed = false;

        void check_scroll_bounds();
    };

}
