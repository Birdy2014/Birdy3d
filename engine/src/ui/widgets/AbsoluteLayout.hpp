#pragma once

#include "ui/Layout.hpp"

namespace Birdy3d {

    class AbsoluteLayout : public Layout {
    public:
        AbsoluteLayout(UIVector pos = UIVector(0), UIVector size = UIVector(0), Placement placement = Placement::BOTTOM_LEFT)
            : Layout(pos, size, placement) { }

        void arrange(glm::mat4 move, glm::vec2 size) override {
            Widget::arrange(move, size);

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
    };

}
