#include "ui/Window.hpp"

#include "core/Input.hpp"
#include "ui/Layout.hpp"
#include "ui/Theme.hpp"

namespace Birdy3d {

    Window::Window(UIVector pos, UIVector size, Theme* theme, std::string name)
        : Widget(pos, size, Placement::BOTTOM_LEFT, theme, name) {
        m_padding = glm::vec4(BORDER_SIZE, BORDER_SIZE, BORDER_SIZE, theme->line_height);
        add_filled_rectangle(0_px, UIVector(100_p, 100_p - theme->line_height), theme->color_bg, Placement::BOTTOM_LEFT);
        add_filled_rectangle(0_px, UIVector(100_p, theme->line_height), theme->color_title_bar, Placement::TOP_LEFT);
        add_rectangle(0_px, 100_p, theme->color_border);
        closeButton = add_filled_rectangle(-4_px, 14_px, "#FF0000", Placement::TOP_RIGHT);
        m_title = add_text(UIVector(10_px, -4_px), theme->font_size, "", theme->color_fg, Placement::TOP_LEFT);
    }

    void Window::toForeground() {
        parent->toForeground(this);
    }

    void Window::on_update() {
        Widget::on_update();

        if (!is_hovering())
            return;

        glm::vec2 localCursorPos = Input::cursorPos() - m_actual_pos;

        hoverDrag = false;
        hoverResizeXL = false;
        hoverResizeXR = false;
        hoverResizeY = false;

        if (!dragging && !resizeXL && !resizeXR && !resizeY && closeButton->contains(localCursorPos)) {
            Input::setCursor(Input::CURSOR_HAND);
            return;
        }

        if (localCursorPos.y >= m_actual_size.y - theme->line_height)
            hoverDrag = true;

        if (localCursorPos.y < m_actual_size.y - theme->line_height) {
            if (localCursorPos.x < BORDER_SIZE)
                hoverResizeXL = true;
            if (localCursorPos.x > m_actual_size.x - BORDER_SIZE)
                hoverResizeXR = true;
            if (localCursorPos.y < BORDER_SIZE)
                hoverResizeY = true;
        }

        // Set cursor
        if (hoverDrag || dragging)
            Input::setCursor(Input::CURSOR_MOVE);
        else if (hoverResizeXL || resizeXL)
            Input::setCursor(Input::CURSOR_HRESIZE);
        else if (hoverResizeXR || resizeXR)
            Input::setCursor(Input::CURSOR_HRESIZE);
        else if (hoverResizeY || resizeY)
            Input::setCursor(Input::CURSOR_VRESIZE);
        else
            Input::setCursor(Input::CURSOR_DEFAULT);

        // Move and resize
        if (dragging) {
            pos = pos + Input::cursorPosOffset();
        }
        glm::vec2 minsize = m_layout->minimal_size(m_children) + glm::vec2(m_padding[0] + m_padding[1], m_padding[2] + m_padding[3]);
        if (resizeXL) {
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
        if (resizeXR) {
            size.x += Input::cursorPosOffset().x;
        }
        if (resizeY) {
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
            dragging = false;
            resizeXL = false;
            resizeXR = false;
            resizeY = false;
            size = m_actual_size;
            return;
        }

        if (closeButton->contains(localCursorPos)) {
            if (callback_close)
                callback_close();
            else
                hidden = true;
            return;
        }

        grab_cursor();

        // Moving
        if (hoverDrag) {
            dragging = true;
            return;
        }

        // Resizing
        if (hoverResizeXL)
            resizeXL = true;
        if (hoverResizeXR)
            resizeXR = true;
        if (hoverResizeY)
            resizeY = true;
    }

    void Window::on_mouse_leave() {
        Input::setCursor(Input::CURSOR_DEFAULT);
    }

}
