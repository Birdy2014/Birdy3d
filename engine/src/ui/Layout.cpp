#include "ui/Layout.hpp"

#include "ui/Widget.hpp"

namespace Birdy3d {

    void EmptyLayout::arrange(const std::list<std::shared_ptr<Widget>>& children, glm::vec2, glm::vec2) const {
        for (auto it = children.rbegin(); it != children.rend(); it++) {
            (*it)->arrange(glm::vec2(0), glm::vec2(0));
        }
    }

    glm::vec2 EmptyLayout::minimal_size(const std::list<std::shared_ptr<Widget>>&) const {
        return glm::vec2(0);
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

}
