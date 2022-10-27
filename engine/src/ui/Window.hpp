#pragma once

#include "ui/Container.hpp"
#include "ui/Rectangle.hpp"
#include "ui/TextRenderer.hpp"

namespace Birdy3d::ui {

    class Window : public Container {
    public:
        int const border_size = 4;
        std::function<void()> callback_close;

        Window(Options);
        void to_foreground();
        glm::ivec2 minimal_size() override;

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
        void on_click(ClickEvent&) override;
        void on_mouse_leave(MouseLeaveEvent&) override;
        glm::ivec2 minimal_window_size();

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
        glm::vec2 m_cursor_down_local_pos;
    };

}
