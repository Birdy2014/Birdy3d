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
        Widget::draw();
        if (child)
            child->draw();
    }

    bool Window::update(bool hover) {
        glm::vec2 localCursorPos = Input::cursorPos() - actualPos;

        hoverDrag = false;
        hoverResizeXL = false;
        hoverResizeXR = false;
        hoverResizeY = false;

        if (hover && !closeButton->contains(localCursorPos)) {
            if (localCursorPos.y > actualSize.y - BAR_HEIGHT)
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
        else
            Input::setCursor(Input::CURSOR_DEFAULT);

        // Move and resize
        int maxSize = BAR_HEIGHT + BORDER_SIZE + 10;
        if (dragging) {
            pos = pos + Input::cursorPosOffset();
        }
        if (resizeXL) {
            size.x -= Input::cursorPosOffset().x;
            if (size.x < maxSize)
                size.x = maxSize;
            else
                pos.x += Input::cursorPosOffset().x;
        }
        if (resizeXR) {
            size.x += Input::cursorPosOffset().x;
            if (size.x < maxSize)
                size.x = maxSize;
        }
        if (resizeY) {
            size.y -= Input::cursorPosOffset().y;
            if (size.y < maxSize)
                size.y = maxSize;
            else
                pos.y += Input::cursorPosOffset().y;
        }
    }

    bool Window::onScroll(InputScrollEvent* event, bool hover) {
        if (child)
            child->_onScroll(event, hover);
        return true;
    }

    bool Window::onClick(InputClickEvent* event, bool hover) {
        if (child && child->_onClick(event, hover))
            hover = false;

        if (event->button != GLFW_MOUSE_BUTTON_LEFT || (!hover && event->action != GLFW_RELEASE))
            return true;

        glm::vec2 localCursorPos = Input::cursorPos() - actualPos;

        if (closeButton->contains(localCursorPos)) {
            return true;
        }

        if (event->action == GLFW_RELEASE) {
            dragging = false;
            resizeXL = false;
            resizeXR = false;
            resizeY = false;
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
            child->_onKey(event, hover);
        return true;
    }

    bool Window::onChar(InputCharEvent* event, bool hover) {
        if (child)
            child->_onChar(event, hover);
        return true;
    }

}
