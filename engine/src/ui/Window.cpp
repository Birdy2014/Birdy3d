#include "ui/Window.hpp"

#include "core/Input.hpp"
#include "ui/Theme.hpp"

namespace Birdy3d {

    Window::Window(UIVector pos, UIVector size, Theme* theme, std::string name)
        : Widget(pos, size, Placement::BOTTOM_LEFT, theme, name) {
        addFilledRectangle(0_px, 100_p, theme->color_bg);
        addRectangle(0_px, 100_p, theme->color_border);
        closeButton = new Rectangle(-2_px, 6_px, "#FF0000", Shape::Type::FILLED, Placement::TOP_RIGHT);
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

    void Window::update() {
        Widget::update();
        if (dragging) {
            pos = pos + Input::cursorPos() - dragStart;
            dragStart = Input::cursorPos();
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

        if (hover && event->action == GLFW_PRESS) {
            dragging = true;
            dragStart = Input::cursorPos();
        } else if (event->action == GLFW_RELEASE) {
            dragging = false;
        }

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
