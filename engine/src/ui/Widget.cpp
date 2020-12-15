#include "ui/Widget.hpp"

#include "core/Application.hpp"
#include "core/Input.hpp"

Widget::Widget(Shader *shader, glm::vec3 pos, Placement placement, float rotation, glm::vec2 scale) {
    this->shader = shader;
    this->pos = pos;
    this->placement = placement;
    this->rot = rotation;
    this->scale = scale;
}

void Widget::addRectangle(glm::ivec2 pos, glm::ivec2 size, glm::vec4 color, float depth) {
    this->rectangles.push_back(Rectangle(this->shader, pos, size, depth, false, color));
}

void Widget::addFilledRectangle(glm::ivec2 pos, glm::ivec2 size, glm::vec4 color, float depth) {
    this->rectangles.push_back(Rectangle(this->shader, pos, size, depth, true, color));
}

void Widget::draw() {
    if (hidden)
        return;

    glm::mat4 move = this->absTransform(true);

    for (Rectangle r : this->rectangles) {
        r.setMove(move);
        r.draw();
    }
    // draw children
    glm::vec2 size = getSize();
    for (Widget *w : children) {
        w->draw();
    }
}

glm::ivec2 Widget::getBottomLeft() {
    glm::ivec2 bottomLeft(Application::getViewportSize());
    for (Rectangle r : this->rectangles) {
        glm::ivec2 rpos = r.getPos();
        if (rpos.x < bottomLeft.x)
            bottomLeft.x = rpos.x;
        if (rpos.y < bottomLeft.y)
            bottomLeft.y = rpos.y;
    }
    return bottomLeft;
}

glm::ivec2 Widget::getTopRight() {
    glm::ivec2 topRight(0);
    for (Rectangle r : this->rectangles) {
        glm::ivec2 rpos = r.getPos();
        glm::ivec2 rsize = r.getSize();
        if (rpos.x + rsize.x > topRight.x)
            topRight.x = rpos.x + rsize.x;
        if (rpos.y + rsize.y > topRight.y)
            topRight.y = rpos.y + rsize.y;
    }
    return topRight;
}

glm::ivec2 Widget::getSize() {
    return this->getTopRight() - this->getBottomLeft();
}

bool Widget::updateEvents() {
    if (hidden)
        return false;

    glm::vec2 size = getSize();
    glm::vec2 viewportSize = Application::getViewportSize();
    glm::vec3 absPos = this->absTransform() * glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);

    // children
    for (Widget *w : this->children) {
        if (w->updateEvents())
            return true;
    }
    // self
    if (Input::buttonPressed(GLFW_MOUSE_BUTTON_LEFT)) {
        glm::vec2 cursorPos = Input::cursorPos();
        glm::ivec2 bottomLeft = this->getBottomLeft() + glm::ivec2(absPos);
        glm::ivec2 topRight = this->getTopRight() + glm::ivec2(absPos);
        if (cursorPos.x > bottomLeft.x && cursorPos.x < topRight.x && cursorPos.y > bottomLeft.y && cursorPos.y < topRight.y) {
            return clickHandler();
        }
    }
    return false;
}

// Widget position relative to the parent
glm::vec2 Widget::getPos() {
    glm::vec2 pos;
    glm::vec2 parentSize;
    glm::vec2 bottomLeft;
    if (this->parent) {
        parentSize = this->parent->getSize();
        bottomLeft = this->parent->getBottomLeft();
    } else {
        parentSize = Application::getViewportSize();
        bottomLeft = glm::vec2(0);
    }
    
    if (placement == Placement::TOP_LEFT || placement == Placement::BOTTOM_LEFT || placement == Placement::CENTER_LEFT) {
        pos.x = this->pos.x + bottomLeft.x;
    } else if (placement == Placement::TOP_RIGHT || placement == Placement::BOTTOM_RIGHT || placement == Placement::CENTER_RIGHT) {
        pos.x = parentSize.x + this->pos.x + bottomLeft.x;
    } else if (placement == Placement::TOP_CENTER || placement == Placement::BOTTOM_CENTER || placement == Placement::CENTER) {
        pos.x = parentSize.x / 2 + this->pos.x + bottomLeft.x;
    }
    if (placement == Placement::TOP_LEFT || placement == Placement::TOP_RIGHT || placement == Placement::TOP_CENTER) {
        pos.y = parentSize.y + this->pos.y + bottomLeft.y;
    } else if (placement == Placement::BOTTOM_LEFT || placement == Placement::BOTTOM_RIGHT || placement == Placement::BOTTOM_CENTER) {
        pos.y = this->pos.y + bottomLeft.y;
    } else if (placement == Placement::CENTER_LEFT || placement == Placement::CENTER_RIGHT || placement == Placement::CENTER) {
        pos.y = parentSize.y / 2 + this->pos.y + bottomLeft.y;
    }
    return pos;
}

glm::mat4 Widget::absTransform(bool normalize) {
    glm::mat4 m(1);
    m = glm::rotate(m, this->rot, glm::vec3(0, 0, 1));
    if (this->parent)
        m = m * this->parent->absTransform();
    m = glm::translate(m, glm::vec3(this->getPos(), 0.0f));
    m = glm::scale(m, glm::vec3(this->scale, 1.0f));
    if (normalize) {
        // Convert the pixel coordinates to normalized coordinates
        glm::vec2 viewportSize = Application::getViewportSize();
        m = glm::ortho(0.0f, viewportSize.x, 0.0f, viewportSize.y) * m;
    }
    return m;
}
