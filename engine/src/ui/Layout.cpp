#include "ui/Layout.hpp"

#include "ui/Widget.hpp"

namespace Birdy3d::ui {

    glm::vec2 Layout::minimal_size(const std::list<std::shared_ptr<Widget>>& children, float, Layout::Direction) const {
        return minimal_size(children);
    };

    float Layout::minimal_size(const std::list<std::shared_ptr<Widget>>& children, Layout::Direction direction) const {
        return direction == Layout::Direction::HORIZONTAL ? minimal_size(children).x : minimal_size(children).y;
    }

    void MaxLayout::arrange(const std::list<std::shared_ptr<Widget>>& children, glm::vec2 pos, glm::vec2 size) const {
        for (auto it = children.rbegin(); it != children.rend(); it++) {
            (*it)->arrange(pos, size);
        }
    }

    glm::vec2 MaxLayout::minimal_size(const std::list<std::shared_ptr<Widget>>& children) const {
        if (children.empty())
            return glm::vec2(0);
        return children.front()->minimal_size();
    }

    void AbsoluteLayout::arrange(const std::list<std::shared_ptr<Widget>>& children, glm::vec2 pos, glm::vec2 size) const {
        for (const auto& child : children) {
            glm::vec2 child_size = child->preferred_size(size);
            glm::vec2 child_pos = pos + child->preferred_position(size, child_size);
            if (child_size.x == 0 || child_size.y == 0)
                child_size = size;
            child->arrange(child_pos, child_size);
        }
    }

    glm::vec2 AbsoluteLayout::minimal_size(const std::list<std::shared_ptr<Widget>>& children [[maybe_unused]]) const {
        return glm::vec2(0);
    }

}
