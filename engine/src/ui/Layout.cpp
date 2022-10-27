#include "ui/Layout.hpp"

#include "ui/Widget.hpp"

namespace Birdy3d::ui {

    glm::ivec2 Layout::minimal_size(std::list<std::shared_ptr<Widget>> const& children, float, Layout::Direction) const
    {
        return minimal_size(children);
    };

    float Layout::minimal_size(std::list<std::shared_ptr<Widget>> const& children, Layout::Direction direction) const
    {
        return direction == Layout::Direction::HORIZONTAL ? minimal_size(children).x : minimal_size(children).y;
    }

    void MaxLayout::arrange(std::list<std::shared_ptr<Widget>> const& children, glm::ivec2 pos, glm::ivec2 size) const
    {
        for (auto it = children.rbegin(); it != children.rend(); it++) {
            (*it)->arrange(pos, size);
        }
    }

    glm::ivec2 MaxLayout::minimal_size(std::list<std::shared_ptr<Widget>> const& children) const
    {
        if (children.empty())
            return glm::ivec2(0);
        return children.front()->minimal_size();
    }

    void AbsoluteLayout::arrange(std::list<std::shared_ptr<Widget>> const& children, glm::ivec2 pos, glm::ivec2 size) const
    {
        for (auto const& child : children) {
            glm::ivec2 child_size = child->preferred_size(size);
            glm::ivec2 child_pos = pos + child->preferred_position(size, child_size);
            if (child_size.x == 0 || child_size.y == 0)
                child_size = size;
            child->arrange(child_pos, child_size);
        }
    }

    glm::ivec2 AbsoluteLayout::minimal_size(std::list<std::shared_ptr<Widget>> const& children [[maybe_unused]]) const
    {
        return glm::ivec2(0);
    }

}
