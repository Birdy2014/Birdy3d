#include "ui/Window.hpp"

#include "core/Input.hpp"
#include "ui/Layout.hpp"
#include "ui/Theme.hpp"

namespace Birdy3d {

    Window::Window(UIVector pos, UIVector size, Theme* theme, std::string name)
        : Widget(pos, size, Placement::BOTTOM_LEFT, theme, name) {
        addFilledRectangle(0_px, UIVector(100_p, 100_p - theme->line_height), theme->color_bg, Placement::BOTTOM_LEFT);
        addFilledRectangle(0_px, UIVector(100_p, theme->line_height), theme->color_title_bar, Placement::TOP_LEFT);
        addRectangle(0_px, 100_p, theme->color_border);
        closeButton = addFilledRectangle(-4_px, 14_px, "#FF0000", Placement::TOP_RIGHT);
        m_title = addText(UIVector(10_px, -4_px), theme->font_size, "", theme->color_fg, Placement::TOP_LEFT);
    }

    void Window::arrange(glm::vec2 pos, glm::vec2 size) {
        Widget::arrange(pos, size);
        if (m_child) {
            m_child->arrange(pos + glm::vec2(BORDER_SIZE, BORDER_SIZE), glm::vec2(size.x - 2 * BORDER_SIZE, size.y - BORDER_SIZE - theme->line_height));
        }
    }

    void Window::toForeground() {
        Layout* layout = dynamic_cast<Layout*>(parent);
        if (layout)
            layout->toForeground(this);
    }

    void Window::draw() {
        if (hidden)
            return;
        Widget::draw();
        if (m_child)
            m_child->draw();
    }

    glm::vec2 Window::minimalSize() {
        glm::vec2 minSelf = glm::vec2(theme->line_height + BORDER_SIZE + 10);
        glm::vec2 minChild(0);
        if (m_child)
            minChild = m_child->minimalSize();
        return minSelf + minChild;
    }

    void Window::set_child(Widget* child) {
        child->set_canvas(canvas);
        m_child = child;
    }

    void Window::set_canvas(Canvas* c) {
        Widget::set_canvas(c);

        if (m_child)
            m_child->set_canvas(c);
    }

    bool Window::update_hover(bool hover) {
        bool success = false;
        if (hidden)
            hover = false;
        if (m_child->update_hover(hover)) {
            hover = false;
            success = true;
        }
        return Widget::update_hover(hover) || success;
    }

    void Window::late_update() {
        if (m_child)
            m_child->late_update();
        Widget::late_update();
    }

    void Window::on_update() {
        if (m_child)
            m_child->on_update();

        if (!is_hovering())
            return;

        glm::vec2 localCursorPos = Input::cursorPos() - actualPos;

        hoverDrag = false;
        hoverResizeXL = false;
        hoverResizeXR = false;
        hoverResizeY = false;

        if (!dragging && !resizeXL && !resizeXR && !resizeY && closeButton->contains(localCursorPos)) {
            Input::setCursor(Input::CURSOR_HAND);
            return;
        }

        if (localCursorPos.y >= actualSize.y - theme->line_height)
            hoverDrag = true;

        if (localCursorPos.y < actualSize.y - theme->line_height) {
            if (localCursorPos.x < BORDER_SIZE)
                hoverResizeXL = true;
            if (localCursorPos.x > actualSize.x - BORDER_SIZE)
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
        if (resizeXL) {
            float diffold = size.x - minimalSize().x;
            size.x -= Input::cursorPosOffset().x;
            float diffnew = size.x - minimalSize().x;
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
            float diffold = size.y - minimalSize().y;
            size.y -= Input::cursorPosOffset().y;
            float diffnew = size.y - minimalSize().y;
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

        glm::vec2 localCursorPos = Input::cursorPos() - actualPos;

        if (event->action == GLFW_RELEASE) {
            ungrab_cursor();
            dragging = false;
            resizeXL = false;
            resizeXR = false;
            resizeY = false;
            size = actualSize;
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
