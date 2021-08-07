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
        m_title = add_text(UIVector(10_px, -4_px), Application::theme->font_size, "", Application::theme->color_fg, Placement::TOP_LEFT);
    }

    void Window::toForeground() {
        parent->toForeground(this);
    }

    void Window::on_update() {
        Widget::on_update();

        if (!is_hovering())
            return;

        glm::vec2 localCursorPos = Input::cursorPos() - m_actual_pos;

        m_hover_drag = false;
        m_hover_resize_xl = false;
        m_hover_resize_xr = false;
        m_hover_resize_y = false;

        if (!m_dragging && !m_resize_xl && !m_resize_xr && !m_resize_y && m_close_button->contains(localCursorPos)) {
            Input::setCursor(Input::CURSOR_HAND);
            return;
        }

        if (localCursorPos.y >= m_actual_size.y - Application::theme->line_height)
            m_hover_drag = true;

        if (localCursorPos.y < m_actual_size.y - Application::theme->line_height) {
            if (localCursorPos.x < BORDER_SIZE)
                m_hover_resize_xl = true;
            if (localCursorPos.x > m_actual_size.x - BORDER_SIZE)
                m_hover_resize_xr = true;
            if (localCursorPos.y < BORDER_SIZE)
                m_hover_resize_y = true;
        }

        // Set cursor
        if (m_hover_drag || m_dragging)
            Input::setCursor(Input::CURSOR_MOVE);
        else if (m_hover_resize_xl || m_resize_xl)
            Input::setCursor(Input::CURSOR_HRESIZE);
        else if (m_hover_resize_xr || m_resize_xr)
            Input::setCursor(Input::CURSOR_HRESIZE);
        else if (m_hover_resize_y || m_resize_y)
            Input::setCursor(Input::CURSOR_VRESIZE);
        else
            Input::setCursor(Input::CURSOR_DEFAULT);

        // Move and resize
        if (m_dragging) {
            pos = pos + Input::cursorPosOffset();
        }
        glm::vec2 minsize = m_layout->minimal_size(m_children) + glm::vec2(m_padding[0] + m_padding[1], m_padding[2] + m_padding[3]);
        if (m_resize_xl) {
            float diffold = size.x - minsize.x;
            size.x -= Input::cursorPosOffset().x;
            float diffnew = size.x - minsize.x;
            if (diffold >= 0 && diffnew >= 0) {
                pos.x += Input::cursorPosOffset().x;
            } else if (diffold >= 0 && diffnew < 0) {
                pos.x += diffold;
            } else if (diffold < 0 && diffnew >= 0) {
                pos.x -= diffnew;
            }
        }
        if (m_resize_xr) {
            size.x += Input::cursorPosOffset().x;
        }
        if (m_resize_y) {
            float diffold = size.y - minsize.y;
            size.y -= Input::cursorPosOffset().y;
            float diffnew = size.y - minsize.y;
            if (diffold >= 0 && diffnew >= 0) {
                pos.y += Input::cursorPosOffset().y;
            } else if (diffold >= 0 && diffnew < 0) {
                pos.y += diffold;
            } else if (diffold < 0 && diffnew >= 0) {
                pos.y -= diffnew;
            }
        }
    }

    void Window::on_click(InputClickEvent* event) {
        if (event->button != GLFW_MOUSE_BUTTON_LEFT)
            return;

        if (event->action == GLFW_PRESS)
            toForeground();

        glm::vec2 localCursorPos = Input::cursorPos() - m_actual_pos;

        if (event->action == GLFW_RELEASE) {
            ungrab_cursor();
            m_dragging = false;
            m_resize_xl = false;
            m_resize_xr = false;
            m_resize_y = false;
            size = m_actual_size;
            return;
        }

        if (m_close_button->contains(localCursorPos)) {
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
        if (m_hover_resize_xl)
            m_resize_xl = true;
        if (m_hover_resize_xr)
            m_resize_xr = true;
        if (m_hover_resize_y)
            m_resize_y = true;
    }

    void Window::on_mouse_leave() {
        Input::setCursor(Input::CURSOR_DEFAULT);
    }

}
