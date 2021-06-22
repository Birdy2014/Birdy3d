#include "ui/Widget.hpp"

#include "core/Input.hpp"
#include "ui/Rectangle.hpp"
#include "ui/TextRenderer.hpp"
#include "ui/Triangle.hpp"

namespace Birdy3d {

    Widget::Widget(UIVector pos, UIVector size, Placement placement, Theme* theme, std::string name)
        : name(name)
        , pos(pos)
        , size(size)
        , placement(placement)
        , theme(theme) { }

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

        glScissor(actualPos.x, actualPos.y, actualSize.x, actualSize.y);

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

    bool Widget::notifyEvent(EventType type, Event* event, bool hover) {
        auto callHandler = [&]() {
            switch (type) {
            case EventType::UPDATE:
                return update(hover);
            case EventType::SCROLL:
                return onScroll((InputScrollEvent*)event, hover);
            case EventType::CLICK:
                return onClick((InputClickEvent*)event, hover);
            case EventType::KEY:
                return onKey((InputKeyEvent*)event, hover);
            case EventType::CHAR:
                return onChar((InputCharEvent*)event, hover);
            }
            return false;
        };

        if (hover) {
            if (hidden)
                hover = false;
            glm::vec2 cursorPos = Input::cursorPos();
            if (cursorPos.x > actualPos.x && cursorPos.y > actualPos.y && cursorPos.x < actualPos.x + actualSize.x && cursorPos.y < actualPos.y + actualSize.y) {
                if (type == EventType::UPDATE && !hoveredLastFrame) {
                    runMouseEnter = true;
                    hoveredLastFrame = true;
                }
                return !(hidden || !callHandler());
            }
        }
        hover = false;
        callHandler();
        if (type == EventType::UPDATE && hoveredLastFrame) {
            onMouseLeave();
            hoveredLastFrame = false;
        }
        return false;
    }

    void Widget::lateUpdate() {
        if (runMouseEnter) {
            onMouseEnter();
            runMouseEnter = false;
        }
    }

    bool Widget::update(bool) {
        return true;
    }

    bool Widget::onScroll(InputScrollEvent*, bool) {
        return true;
    }

    bool Widget::onClick(InputClickEvent*, bool) {
        return true;
    }

    bool Widget::onKey(InputKeyEvent*, bool) {
        return true;
    }

    bool Widget::onChar(InputCharEvent*, bool) {
        return true;
    }

    void Widget::onMouseEnter() { }
    void Widget::onMouseLeave() { }
}
