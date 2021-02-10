#include "ui/Widget.hpp"

#include "core/Application.hpp"
#include "core/Input.hpp"
#include "core/RessourceManager.hpp"
#include "ui/Rectangle.hpp"
#include "ui/Triangle.hpp"

Widget::Widget(glm::vec2 pos, Placement placement, float rotation) {
    this->pos = pos;
    this->placement = placement;
    this->rot = rotation;
}

void Widget::addRectangle(glm::vec2 pos, glm::vec2 size, glm::vec4 color) {
    this->shapes.push_back(new Rectangle(pos, size, color, Shape::OUTLINE));
}

void Widget::addFilledRectangle(glm::vec2 pos, glm::vec2 size, glm::vec4 color) {
    this->shapes.push_back(new Rectangle(pos, size, color, Shape::FILLED));
}

void Widget::addTriangle(glm::vec2 pos, glm::vec2 size, glm::vec4 color) {
    this->shapes.push_back(new Triangle(pos, size, color, Shape::OUTLINE));
}

void Widget::addFilledTriangle(glm::vec2 pos, glm::vec2 size, glm::vec4 color) {
    this->shapes.push_back(new Triangle(pos, size, color, Shape::FILLED));
}

void Widget::addText(glm::vec2 pos, float fontSize, std::string text, glm::vec4 color) {
    this->texts.push_back(Widget::Text{pos, fontSize, text, color});
}

void Widget::draw() {
    if (hidden)
        return;

    glm::mat4 move = normalizedMove();

    // draw self
    for (Shape *s : this->shapes) {
        s->draw(move);
    }

    for (Widget::Text t : this->texts) {
        Application::getTextRenderer()->renderText(t.text, t.pos.x, t.pos.y, t.fontSize, t.color, move);
    }

    // draw children
    for (Widget *w : children) {
        w->draw();
    }
}

glm::ivec2 Widget::getBottomLeft() {
    glm::ivec2 bottomLeft(Application::getViewportSize());
    for (Shape *s : this->shapes) {
        glm::ivec2 rpos = s->position();
        if (rpos.x < bottomLeft.x)
            bottomLeft.x = rpos.x;
        if (rpos.y < bottomLeft.y)
            bottomLeft.y = rpos.y;
    }
    return bottomLeft;
}

glm::ivec2 Widget::getTopRight() {
    glm::ivec2 topRight(0);
    for (Shape *s : this->shapes) {
        glm::ivec2 rpos = s->position();
        glm::ivec2 rsize = s->size();
        if (rpos.x + rsize.x > topRight.x)
            topRight.x = rpos.x + rsize.x;
        if (rpos.y + rsize.y > topRight.y)
            topRight.y = rpos.y + rsize.y;
    }
    return topRight;
}

// TODO: size variable (with 0 for no preferred size) instead of this function
glm::ivec2 Widget::getSize() {
    //return this->getTopRight() - this->getBottomLeft();
    return this->getTopRight();
}

// TODO: eventdispatcher to notify about the individual shapes separately
bool Widget::updateEvents() {
    if (hidden)
        return false;

    // children
    for (Widget *w : this->children) {
        if (w->updateEvents())
            return true;
    }

    // self
    if (Input::buttonPressed(GLFW_MOUSE_BUTTON_LEFT)) {
        glm::vec2 absPos = move * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
        glm::vec2 cursorPos = Input::cursorPos();
        for (Shape *s : this->shapes) {
            if (s->contains(cursorPos - absPos))
                return clickHandler();
        }
    }
    return false;
}

// TODO: use placement to position shapes additionally to widgets, getShapeByName
// Widget position relative to the parent
glm::vec2 Widget::getPos(glm::vec2 parentSize) {
    glm::vec2 pos;

    if (placement == Placement::TOP_LEFT || placement == Placement::BOTTOM_LEFT || placement == Placement::CENTER_LEFT) {
        pos.x = this->pos.x;
    } else if (placement == Placement::TOP_RIGHT || placement == Placement::BOTTOM_RIGHT || placement == Placement::CENTER_RIGHT) {
        pos.x = parentSize.x + this->pos.x;
    } else if (placement == Placement::TOP_CENTER || placement == Placement::BOTTOM_CENTER || placement == Placement::CENTER) {
        pos.x = parentSize.x / 2 + this->pos.x;
    }
    if (placement == Placement::TOP_LEFT || placement == Placement::TOP_RIGHT || placement == Placement::TOP_CENTER) {
        pos.y = parentSize.y + this->pos.y;
    } else if (placement == Placement::BOTTOM_LEFT || placement == Placement::BOTTOM_RIGHT || placement == Placement::BOTTOM_CENTER) {
        pos.y = this->pos.y;
    } else if (placement == Placement::CENTER_LEFT || placement == Placement::CENTER_RIGHT || placement == Placement::CENTER) {
        pos.y = parentSize.y / 2 + this->pos.y;
    }
    return pos;
}

// TODO: arrange shapes based on their placement/unit values
void Widget::arrange(glm::mat4 move, glm::vec2 size) {
    this->move = move;
    this->size = size; // TODO: use size to resize shapes
    for (Widget *child : children) {
        glm::mat4 m = move;
        m = glm::translate(m, glm::vec3(child->getPos(size), 0.0f));
        m = glm::rotate(m, child->rot, glm::vec3(0, 0, 1));
        glm::vec2 childSize = child->getSize();
        if (childSize.x == 0 || childSize.y == 0)
            childSize = size;
        child->arrange(m, childSize);
    }
}

glm::mat4 Widget::normalizedMove() {
    glm::vec2 viewportSize = Application::getViewportSize();
    return glm::ortho(0.0f, viewportSize.x, 0.0f, viewportSize.y) * move;
}
