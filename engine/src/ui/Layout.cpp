#pragma once

#include "ui/Layout.hpp"
#include "ui/Shape.hpp"
#include "ui/TextRenderer.hpp"

namespace Birdy3d {

    Layout::Layout(UIVector pos, UIVector size, Placement placement)
        : Widget(pos, size, placement) { }

    void Layout::addChild(Widget* w) {
        children.push_back(w);
    }

    void Layout::draw() {
        Widget::draw();
        for (Widget* w : children) {
            w->draw();
        }
    }

    void Layout::arrange(glm::mat4 move, glm::vec2 size) {
        this->move = move;

        for (Shape* s : shapes) {
            s->parentSize(size);
        }

        for (Text* t : texts) {
            t->calcPos(size);
        }

        for (Widget* child : children) {
            glm::mat4 m = move;
            m = glm::translate(m, glm::vec3(child->preferredPosition(size), 0.0f));
            m = glm::rotate(m, child->rot, glm::vec3(0, 0, 1));
            glm::vec2 childSize = child->preferredSize(size);
            if (childSize.x == 0 || childSize.y == 0)
                childSize = size;
            child->arrange(m, childSize);
        }
    }

    bool Layout::updateEvents() {
        if (hidden)
            return false;

        for (Widget* w : this->children) {
            if (w->updateEvents())
                return true;
        }

        return Widget::updateEvents();
    }

}
