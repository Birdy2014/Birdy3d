#pragma once

#include "ui/Layout.hpp"

namespace Birdy3d {

    class AbsoluteLayout : public Layout {
    public:
        AbsoluteLayout(UIVector pos = UIVector(0), UIVector size = UIVector(0), Placement placement = Placement::BOTTOM_LEFT)
            : Layout(pos, size, placement) { }

        void arrange(glm::vec2 pos, glm::vec2 size) override {
            Widget::arrange(pos, size);

            for (Widget* child : children) {
                glm::vec2 childSize = child->preferredSize(size);
                glm::vec2 childPos = child->preferredPosition(size, childSize);
                if (childSize.x == 0 || childSize.y == 0)
                    childSize = size;
                child->arrange(childPos, childSize);
            }
        }
    };

}
