#include "ui/Widget.hpp"

#include "core/Input.hpp"
#include "ui/Rectangle.hpp"
#include "ui/TextRenderer.hpp"
#include "ui/Triangle.hpp"

namespace Birdy3d {

    Widget::Widget(UIVector pos, UIVector size, Placement placement, float rotation, Theme* theme, std::string name)
        : pos(pos)
        , size(size)
        , placement(placement)
        , rot(rotation)
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

    // TODO: eventdispatcher to notify about the individual shapes separately
    bool Widget::updateEvents() {
        if (hidden)
            return false;

        hover = false;
        glm::vec2 absPos = move * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
        glm::vec2 cursorPos = Input::cursorPos();
        for (Shape* s : this->shapes) {
            if (s->contains(cursorPos - absPos)) {
                hover = true;
                break;
            }
        }
        return hover;
    }

    glm::vec2 Widget::preferredPosition(glm::vec2 parentSize) {
        return Utils::getRelativePosition(this->pos, actualSize, parentSize, this->placement);
    }

    glm::vec2 Widget::minimalSize() {
        return size.toPixels();
    }

    glm::vec2 Widget::preferredSize(glm::vec2 parentSize) {
        return glm::max(size.toPixels(parentSize), minimalSize());
    }

    void Widget::arrange(glm::mat4 move, glm::vec2 size) {
        this->move = move;
        this->actualSize = size;

        for (Shape* s : shapes) {
            s->parentSize(size);
        }
    }

    glm::mat4 Widget::normalizedMove() {
        glm::vec2 viewportSize = Application::getViewportSize();
        return glm::ortho(0.0f, viewportSize.x, 0.0f, viewportSize.y) * move;
    }

}
