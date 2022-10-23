#include "ui/Layout.hpp"

#include "ui/Widget.hpp"

namespace Birdy3d::ui {

    glm::ivec2 Layout::minimal_size(const std::list<std::shared_ptr<Widget>>& children, float, Layout::Direction) const {
        return minimal_size(children);
    };

    float Layout::minimal_size(const std::list<std::shared_ptr<Widget>>& children, Layout::Direction direction) const {
        return direction == Layout::Direction::HORIZONTAL ? minimal_size(children).x : minimal_size(children).y;
    }

    void MaxLayout::arrange(const std::list<std::shared_ptr<Widget>>& children, glm::ivec2 pos, glm::ivec2 size) const {
        for (auto it = children.rbegin(); it != children.rend(); it++) {
            (*it)->arrange(pos, size);
        }
    }

    glm::ivec2 MaxLayout::minimal_size(const std::list<std::shared_ptr<Widget>>& children) const {
        if (children.empty())
            return glm::ivec2(0);
        return children.front()->minimal_size();
    }

    void AbsoluteLayout::arrange(const std::list<std::shared_ptr<Widget>>& children, glm::ivec2 pos, glm::ivec2 size) const {
        for (const auto& child : children) {
            glm::ivec2 child_size = child->preferred_size(size);
            glm::ivec2 child_pos = pos + child->preferred_position(size, child_size);
            if (child_size.x == 0 || child_size.y == 0)
                child_size = size;
            child->arrange(child_pos, child_size);
        }
    }

    glm::ivec2 AbsoluteLayout::minimal_size(const std::list<std::shared_ptr<Widget>>& children [[maybe_unused]]) const {
        return glm::ivec2(0);
    }

}
