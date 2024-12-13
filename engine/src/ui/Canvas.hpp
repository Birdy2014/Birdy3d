#pragma once

#include "events/Forward.hpp"
#include "ui/Container.hpp"

namespace Birdy3d::ui {

    class Canvas : public Container {
    public:
        bool updated = false;

        Canvas();

        void update();
        void draw_canvas();

        Widget* hovering_widget() { return m_hovering_widget; }
        Widget* focused_widget() { return m_focused_widget; }
        Widget* last_focused_widget() { return m_last_focused_widget; }
        bool cursor_grabbed() { return m_cursor_grabbed; }

        void set_focused(Widget* widget);
        void set_hovering(Widget* widget);
        void set_cursor_grabbed(Widget* widget, bool grabbed);
        void unfocus();
        void start_drag(std::any data);

    private:
        Widget* m_hovering_widget = nullptr;
        Widget* m_focused_widget = nullptr;
        Widget* m_last_focused_widget = nullptr;
        bool m_cursor_grabbed = false;

        bool m_dragging = false;
        std::any m_dragging_value;

        void on_scroll_raw(events::InputScrollEvent const& event);
        void on_click_raw(events::InputClickEvent const& event);
        void on_key_raw(events::InputKeyEvent const& event);
        void on_char_raw(events::InputCharEvent const& event);
    };

}
