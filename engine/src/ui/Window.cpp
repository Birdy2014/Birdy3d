#include "ui/Window.hpp"

#include "core/Input.hpp"
#include "ui/Layout.hpp"
#include "ui/Theme.hpp"

namespace Birdy3d {

    Window::Window(UIVector pos, UIVector size, std::string name)
        : Widget(pos, size, Placement::BOTTOM_LEFT, name) {
        m_padding = glm::vec4(BORDER_SIZE, BORDER_SIZE, BORDER_SIZE, Application::theme->line_height);
        add_filled_rectangle(0_px, UIVector(100_p, 100_p - Application::theme->line_height), Application::theme->color_bg, Placement::BOTTOM_LEFT);
        add_filled_rectangle(0_px, UIVector(100_p, Application::theme->line_height), Application::theme->color_title_bar, Placement::TOP_LEFT);
        add_rectangle(0_px, 100_p, Application::theme->color_border);
        m_close_button = add_filled_rectangle(-4_px, 14_px, "#FF0000", Placement::TOP_RIGHT);
        m_title = add_text(UIVector(10_px, 0_px), Application::theme->font_size, "", Application::theme->color_fg, Placement::TOP_LEFT);
    }

    void Window::toForeground() {
        parent->to_foreground(this);
    }

    void Window::on_update() {
        Widget::on_update();

        m_dragged = false;

        if (!is_hovering())
            return;

        glm::vec2 local_cursor_pos = Input::cursor_pos() - m_actual_pos;

        m_hover_drag = false;
        m_hover_resize_x_left = false;
        m_hover_resize_x_right = false;
        m_hover_resize_y_top = false;
        m_hover_resize_y_bottom = false;

        if (!m_dragging && !m_resize_x_left && !m_resize_x_right && !m_resize_y_top && !m_resize_y_bottom && m_close_button->contains(local_cursor_pos)) {
            Input::set_cursor(Input::CURSOR_HAND);
            return;
        }

        if (local_cursor_pos.x < BORDER_SIZE)
            m_hover_resize_x_left = true;
        if (local_cursor_pos.x > m_actual_size.x - BORDER_SIZE)
            m_hover_resize_x_right = true;
        if (local_cursor_pos.y > m_actual_size.y - BORDER_SIZE)
            m_hover_resize_y_top = true;
        if (local_cursor_pos.y < BORDER_SIZE)
            m_hover_resize_y_bottom = true;

        if (local_cursor_pos.y >= m_actual_size.y - Application::theme->line_height && local_cursor_pos.y <= m_actual_size.y - BORDER_SIZE && local_cursor_pos.x >= BORDER_SIZE && local_cursor_pos.x <= m_actual_size.x - BORDER_SIZE)
            m_hover_drag = true;

        // Set cursor
        if (m_dragging)
            Input::set_cursor(Input::CURSOR_MOVE);
        else if (m_resize_x_left && m_resize_y_top)
            Input::set_cursor(Input::CURSOR_TOP_LEFT_RESIZE);
        else if (m_resize_x_right && m_resize_y_top)
            Input::set_cursor(Input::CURSOR_TOP_RIGHT_RESIZE);
        else if (m_resize_x_left && m_resize_y_bottom)
            Input::set_cursor(Input::CURSOR_BOTTOM_LEFT_RESIZE);
        else if (m_resize_x_right && m_resize_y_bottom)
            Input::set_cursor(Input::CURSOR_BOTTOM_RIGHT_RESIZE);
        else if (m_resize_x_left || m_resize_x_right)
            Input::set_cursor(Input::CURSOR_HRESIZE);
        else if (m_resize_y_top || m_resize_y_bottom)
            Input::set_cursor(Input::CURSOR_VRESIZE);
        else if (m_hover_drag)
            Input::set_cursor(Input::CURSOR_MOVE);
        else if (m_hover_resize_x_left && m_hover_resize_y_top)
            Input::set_cursor(Input::CURSOR_TOP_LEFT_RESIZE);
        else if (m_hover_resize_x_right && m_hover_resize_y_top)
            Input::set_cursor(Input::CURSOR_TOP_RIGHT_RESIZE);
        else if (m_hover_resize_x_left && m_hover_resize_y_bottom)
            Input::set_cursor(Input::CURSOR_BOTTOM_LEFT_RESIZE);
        else if (m_hover_resize_x_right && m_hover_resize_y_bottom)
            Input::set_cursor(Input::CURSOR_BOTTOM_RIGHT_RESIZE);
        else if (m_hover_resize_x_left || m_hover_resize_x_right)
            Input::set_cursor(Input::CURSOR_HRESIZE);
        else if (m_hover_resize_y_top || m_hover_resize_y_bottom)
            Input::set_cursor(Input::CURSOR_VRESIZE);
        else
            Input::set_cursor(Input::CURSOR_DEFAULT);

        // Move and resize
        if (m_dragging) {
            pos = pos + Input::cursor_pos_offset();
            m_dragged = true;
        }
        glm::vec2 minsize = m_layout->minimal_size(m_children) + glm::vec2(m_padding[0] + m_padding[1], m_padding[2] + m_padding[3]);
        if (m_resize_x_left) {
            float diffold = size.x - minsize.x;
            size.x -= Input::cursor_pos_offset().x;
            float diffnew = size.x - minsize.x;
            if (diffold >= 0 && diffnew >= 0) {
                pos.x += Input::cursor_pos_offset().x;
            } else if (diffold >= 0 && diffnew < 0) {
                pos.x += diffold;
            } else if (diffold < 0 && diffnew >= 0) {
                pos.x -= diffnew;
            }
        }
        if (m_resize_x_right) {
            size.x += Input::cursor_pos_offset().x;
        }
        if (m_resize_y_top) {
            size.y += Input::cursor_pos_offset().y;
        }
        if (m_resize_y_bottom) {
            float diffold = size.y - minsize.y;
            size.y -= Input::cursor_pos_offset().y;
            float diffnew = size.y - minsize.y;
            if (diffold >= 0 && diffnew >= 0) {
                pos.y += Input::cursor_pos_offset().y;
            } else if (diffold >= 0 && diffnew < 0) {
                pos.y += diffold;
            } else if (diffold < 0 && diffnew >= 0) {
                pos.y -= diffnew;
            }
        }
        m_actual_pos = pos;
        m_actual_size = glm::vec2(std::max(size.x.to_pixels(), minimal_size().x), std::max(size.y.to_pixels(), minimal_size().y));
    }

    void Window::on_click(InputClickEvent* event) {
        if (event->button != GLFW_MOUSE_BUTTON_LEFT)
            return;

        if (event->action == GLFW_PRESS)
            toForeground();

        glm::vec2 local_cursor_pos = Input::cursor_pos() - m_actual_pos;

        if (event->action == GLFW_RELEASE) {
            ungrab_cursor();
            m_dragging = false;
            m_resize_x_left = false;
            m_resize_x_right = false;
            m_resize_y_top = false;
            m_resize_y_bottom = false;
            size = m_actual_size;
            return;
        }

        if (m_close_button->contains(local_cursor_pos)) {
            if (callback_close)
                callback_close();
            return;
        }

        grab_cursor();

        // Moving
        if (m_hover_drag) {
            m_dragging = true;
            return;
        }

        // Resizing
        if (m_hover_resize_x_left)
            m_resize_x_left = true;
        if (m_hover_resize_x_right)
            m_resize_x_right = true;
        if (m_hover_resize_y_top)
            m_resize_y_top = true;
        if (m_hover_resize_y_bottom)
            m_resize_y_bottom = true;
    }

    void Window::on_mouse_leave() {
        Input::set_cursor(Input::CURSOR_DEFAULT);
    }

}
