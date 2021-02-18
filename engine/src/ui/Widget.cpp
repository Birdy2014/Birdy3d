#include "ui/Widget.hpp"

#include "core/Application.hpp"
#include "core/Input.hpp"
#include "ui/Rectangle.hpp"
#include "ui/TextRenderer.hpp"
#include "ui/Triangle.hpp"

namespace Birdy3d {

    Widget::Widget(glm::vec2 pos, glm::vec2 size, Placement placement, Unit unit, float rotation) {
        this->pos = pos;
        this->size = size;
        this->placement = placement;
        this->rot = rotation;
        this->unit = unit;
    }

    void Widget::addRectangle(glm::vec2 pos, glm::vec2 size, glm::vec4 color, Placement placement, Unit unit) {
        this->shapes.push_back(new Rectangle(pos, size, color, Shape::OUTLINE, placement, unit));
    }

    void Widget::addFilledRectangle(glm::vec2 pos, glm::vec2 size, glm::vec4 color, Placement placement, Unit unit) {
        this->shapes.push_back(new Rectangle(pos, size, color, Shape::FILLED, placement, unit));
    }

    void Widget::addTriangle(glm::vec2 pos, glm::vec2 size, glm::vec4 color) {
        this->shapes.push_back(new Triangle(pos, size, color, Shape::OUTLINE));
    }

    void Widget::addFilledTriangle(glm::vec2 pos, glm::vec2 size, glm::vec4 color) {
        this->shapes.push_back(new Triangle(pos, size, color, Shape::FILLED));
    }

    void Widget::addText(glm::vec2 pos, float fontSize, std::string text, glm::vec4 color, Placement placement) {
        this->texts.push_back(new Text(pos, fontSize, text, color, placement, Application::getTextRenderer()));
    }

    void Widget::draw() {
        if (hidden)
            return;

        glm::mat4 move = normalizedMove();

        for (Shape* s : this->shapes) {
            s->draw(move);
        }

        for (Text* t : this->texts) {
            t->render(move);
        }
    }

    // TODO: eventdispatcher to notify about the individual shapes separately
    bool Widget::updateEvents() {
        if (hidden)
            return false;

        if (Input::buttonPressed(GLFW_MOUSE_BUTTON_LEFT)) {
            glm::vec2 absPos = move * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
            glm::vec2 cursorPos = Input::cursorPos();
            for (Shape* s : this->shapes) {
                if (s->contains(cursorPos - absPos))
                    return clickHandler();
            }
        }
        return false;
    }

    glm::vec2 Widget::pixelPosition(glm::vec2 parentSize) {
        return Utils::getRelativePosition(this->pos, pixelSize(parentSize), parentSize, this->placement, this->unit);
    }

    glm::vec2 Widget::pixelSize(glm::vec2 parentSize) {
        if (glm::length(size) == 0)
            return parentSize;
        return Utils::convertToPixels(size, parentSize, unit);
    }

    // TODO: arrange shapes based on their placement/unit values
    void Widget::arrange(glm::mat4 move, glm::vec2 size) {
        this->move = move;

        for (Shape* s : shapes) {
            s->parentSize(size);
        }

        for (Text* t : texts) {
            t->calcPos(size);
        }
    }

    glm::mat4 Widget::normalizedMove() {
        glm::vec2 viewportSize = Application::getViewportSize();
        return glm::ortho(0.0f, viewportSize.x, 0.0f, viewportSize.y) * move;
    }

}
