#pragma once

#include "events/InputEvents.hpp"
#include "ui/Rectangle.hpp"
#include "ui/TextRenderer.hpp"
#include "ui/Widget.hpp"

namespace Birdy3d {

    class Window : public Widget {
    public:
        const int BORDER_SIZE = 4;
        std::function<void()> callback_close;

        Window(UIVector pos = UIVector(0_px), UIVector size = UIVector(0_px), std::string name = "");
        void toForeground();

        std::string title() { return m_title->text(); }
        void title(std::string title) { m_title->text(title); }
        bool dragging() { return m_dragging; }
        bool resizing() { return m_resize_x_left || m_resize_x_right || m_resize_y_top || m_resize_y_bottom; }
        bool resizing_left() { return m_resize_x_left; }
        bool resizing_right() { return m_resize_x_right; }
        bool resizing_top() { return m_resize_y_top; }
        bool resizing_bottom() { return m_resize_y_bottom; }
        bool dragged() { return m_dragged; }

    protected:
        void on_update() override;
        void on_click(InputClickEvent* event) override;
        void on_mouse_leave() override;

    private:
        Rectangle* m_close_button;
        Text* m_title;
        bool m_hover_drag = false;
        bool m_hover_resize_x_left = false;
        bool m_hover_resize_x_right = false;
        bool m_hover_resize_y_top = false;
        bool m_hover_resize_y_bottom = false;
        bool m_dragging = false;
        bool m_resize_x_left = false;
        bool m_resize_x_right = false;
        bool m_resize_y_top = false;
        bool m_resize_y_bottom = false;
        bool m_dragged = false;
    };

}
