#include "ui/Widget.hpp"

#include "core/Input.hpp"
#include "ui/Rectangle.hpp"
#include "ui/TextRenderer.hpp"
#include "ui/Triangle.hpp"

namespace Birdy3d {

    Widget::Widget(UIVector pos, UIVector size, Placement placement, Theme* theme, std::string name)
        : pos(pos)
        , size(size)
        , placement(placement)
        , theme(theme)
        , name(name) { }

    Widget::~Widget() {
        for (Shape* s : shapes) {
            delete s;
        }
    }

    void Widget::addRectangle(UIVector pos, UIVector size, Color color, Placement placement) {
        this->shapes.push_back(new Rectangle(pos, size, color, Shape::OUTLINE, placement));
    }

    void Widget::addFilledRectangle(UIVector pos, UIVector size, Color color, Placement placement) {
        this->shapes.push_back(new Rectangle(pos, size, color, Shape::FILLED, placement));
    }

    void Widget::addTriangle(UIVector pos, UIVector size, Color color) {
        this->shapes.push_back(new Triangle(pos, size, color, Shape::OUTLINE));
    }

    void Widget::addFilledTriangle(UIVector pos, UIVector size, Color color) {
        this->shapes.push_back(new Triangle(pos, size, color, Shape::FILLED));
    }

    void Widget::addText(UIVector pos, float fontSize, std::string text, Color color, Placement placement) {
        this->shapes.push_back(new Text(pos, fontSize, text, color, placement, Application::getTextRenderer()));
    }

    void Widget::draw() {
        if (hidden)
            return;

        glm::mat4 move = normalizedMove();

        for (Shape* s : this->shapes) {
            s->draw(move);
        }
    }

    glm::vec2 Widget::preferredPosition(glm::vec2 parentSize, glm::vec2 size) {
        return Utils::getRelativePosition(this->pos, size, parentSize, this->placement);
    }

    glm::vec2 Widget::minimalSize() {
        return size.toPixels();
    }

    glm::vec2 Widget::preferredSize(glm::vec2 parentSize) {
        return glm::max(size.toPixels(parentSize), minimalSize());
    }

    void Widget::arrange(glm::vec2 pos, glm::vec2 size) {
        this->actualPos = pos;
        this->actualSize = size;

        for (Shape* s : shapes) {
            s->parentSize(size);
        }
    }

    glm::mat4 Widget::normalizedMove() {
        glm::vec2 viewportSize = Application::getViewportSize();
        glm::mat4 move = glm::ortho(0.0f, viewportSize.x, 0.0f, viewportSize.y);
        return glm::translate(move, glm::vec3(actualPos, 0.0f));
    }

    bool Widget::_update(bool hover) {
        if (hover) {
            if (hidden)
                hover = false;
            if (shapes.empty())
                return update(hover);
            glm::vec2 cursorPos = Input::cursorPos();
            for (Shape* s : this->shapes) {
                if (s->contains(cursorPos - actualPos)) {
                    return update(hover);
                }
            }
        }
        update(false);
        return false;
    }

    bool Widget::_onScroll(InputScrollEvent* event, bool hover) {
        if (hover) {
            if (hidden)
                hover = false;
            if (shapes.empty())
                return onScroll(event, hover);
            glm::vec2 cursorPos = Input::cursorPos();
            for (Shape* s : this->shapes) {
                if (s->contains(cursorPos - actualPos)) {
                    return onScroll(event, hover);
                }
            }
        }
        onScroll(event, false);
        return false;
    }

    bool Widget::_onClick(InputClickEvent* event, bool hover) {
        if (hover) {
            if (hidden)
                hover = false;
            if (shapes.empty())
                return onClick(event, hover);
            glm::vec2 cursorPos = Input::cursorPos();
            for (Shape* s : shapes) {
                if (s->contains(cursorPos - actualPos)) {
                    return onClick(event, hover);
                }
            }
        }
        onClick(event, false);
        return false;
    }

    bool Widget::_onKey(InputKeyEvent* event, bool hover) {
        if (hover) {
            if (hidden)
                hover = false;
            if (shapes.empty())
                return onKey(event, hover);
            glm::vec2 cursorPos = Input::cursorPos();
            for (Shape* s : shapes) {
                if (s->contains(cursorPos - actualPos)) {
                    return onKey(event, hover);
                }
            }
        }
        onKey(event, false);
        return false;
    }

    bool Widget::_onChar(InputCharEvent* event, bool hover) {
        if (hover) {
            if (hidden)
                hover = false;
            if (shapes.empty())
                return onChar(event, hover);
            glm::vec2 cursorPos = Input::cursorPos();
            for (Shape* s : this->shapes) {
                if (s->contains(cursorPos - actualPos)) {
                    return onChar(event, hover);
                }
            }
        }
        onChar(event, false);
        return false;
    }

    bool Widget::update(bool hover) {
        return false;
    }

    bool Widget::onScroll(InputScrollEvent* event, bool hover) {
        return false;
    }

    bool Widget::onClick(InputClickEvent* event, bool hover) {
        return false;
    }

    bool Widget::onKey(InputKeyEvent* event, bool hover) {
        return false;
    }

    bool Widget::onChar(InputCharEvent* event, bool hover) {
        return false;
    }

}
