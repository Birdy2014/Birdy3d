#include "ui/Window.hpp"

#include "core/Input.hpp"
#include "ui/Theme.hpp"

namespace Birdy3d {

    Window::Window(UIVector pos, UIVector size, Theme* theme, std::string name)
        : Widget(pos, size, Placement::BOTTOM_LEFT, theme, name) {
        addFilledRectangle(0_px, 100_p, theme->color_bg);
        addRectangle(0_px, 100_p, theme->color_border);
        closeButton = new Rectangle(-2_px, Unit(BAR_HEIGHT - 2), "#FF0000", Shape::Type::FILLED, Placement::TOP_RIGHT);
        shapes.push_back(closeButton);
    }

    void Window::arrange(glm::vec2 pos, glm::vec2 size) {
        Widget::arrange(pos, size);
        if (child) {
            child->arrange(pos + glm::vec2(BORDER_SIZE, BORDER_SIZE), glm::vec2(size.x - 2 * BORDER_SIZE, size.y - BORDER_SIZE - BAR_HEIGHT));
        }
    }

    void Window::draw() {
        if (hidden)
            return;
        Widget::draw();
        if (child)
            child->draw();
    }

    glm::vec2 Window::minimalSize() {
        glm::vec2 minSelf = glm::vec2(BAR_HEIGHT + BORDER_SIZE + 10);
        glm::vec2 minChild = child->minimalSize();
        return glm::vec2(std::max(minSelf.x, minChild.x), std::max(minSelf.y, minChild.y));
    }

    void Window::lateUpdate() {
        Widget::lateUpdate();
        if (child)
            child->lateUpdate();
    }

    bool Window::update(bool hover) {
        if (child && !dragging && !resizeXL && !resizeXR && !resizeY)
            if (child->notifyEvent(EventType::UPDATE, nullptr, hover))
                return true;

        glm::vec2 localCursorPos = Input::cursorPos() - actualPos;

        hoverDrag = false;
        hoverResizeXL = false;
        hoverResizeXR = false;
        hoverResizeY = false;

        if (hover && !closeButton->contains(localCursorPos)) {
            if (localCursorPos.y >= actualSize.y - BAR_HEIGHT)
                hoverDrag = true;

            if (localCursorPos.y < actualSize.y - BAR_HEIGHT) {
                if (localCursorPos.x < BORDER_SIZE)
                    hoverResizeXL = true;
                if (localCursorPos.x > actualSize.x - BORDER_SIZE)
                    hoverResizeXR = true;
                if (localCursorPos.y < BORDER_SIZE)
                    hoverResizeY = true;
            }
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
        else if (hover)
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
            float diffold = size.y - minimalSize().x;
            size.y -= Input::cursorPosOffset().y;
            float diffnew = size.y - minimalSize().x;
            if (diffold >= 0 && diffnew >= 0) {
                pos.y += Input::cursorPosOffset().y;
            } else if (diffold >= 0 && diffnew < 0) {
                pos.y += diffold;
            } else if (diffold < 0 && diffnew >= 0) {
                pos.y -= diffnew;
            }
        }

        return true;
    }

    bool Window::onScroll(InputScrollEvent* event, bool hover) {
        if (child)
            child->notifyEvent(EventType::SCROLL, event, hover);
        return true;
    }

    bool Window::onClick(InputClickEvent* event, bool hover) {
        if (child && child->notifyEvent(EventType::CLICK, event, hover))
            hover = false;

        if (event->button != GLFW_MOUSE_BUTTON_LEFT || (!hover && event->action != GLFW_RELEASE))
            return true;

        glm::vec2 localCursorPos = Input::cursorPos() - actualPos;

        if (closeButton->contains(localCursorPos)) {
            return true;
        }

        if (event->action == GLFW_RELEASE && (dragging || resizeXL || resizeXR || resizeY)) {
            dragging = false;
            resizeXL = false;
            resizeXR = false;
            resizeY = false;
            size = actualSize;
            Input::setCursor(Input::CURSOR_DEFAULT);
            return true;
        }

        // Moving
        if (hoverDrag) {
            dragging = true;
            return true;
        }

        // Resizing
        if (hoverResizeXL)
            resizeXL = true;
        if (hoverResizeXR)
            resizeXR = true;
        if (hoverResizeY)
            resizeY = true;

        return true;
    }

    bool Window::onKey(InputKeyEvent* event, bool hover) {
        if (child)
            child->notifyEvent(EventType::KEY, event, hover);
        return true;
    }

    bool Window::onChar(InputCharEvent* event, bool hover) {
        if (child)
            child->notifyEvent(EventType::CHAR, event, hover);
        return true;
    }

    void Window::onMouseLeave() {
        Input::setCursor(Input::CURSOR_DEFAULT);
    }

}
