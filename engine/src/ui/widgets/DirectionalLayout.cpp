#include "ui/widgets/DirectionalLayout.hpp"

namespace Birdy3d {

    DirectionalLayout::DirectionalLayout(Direction dir, Placement placement, UIVector size, float gap)
        : Layout(UIVector(0), size, placement)
        , dir(dir)
        , gap(gap) {
    }

    void DirectionalLayout::arrange(glm::vec2 pos, glm::vec2 size) {
        Widget::arrange(pos, size);

        std::list<Widget*> smallerWidgets = m_children;
        float gapps = gap * (m_children.size() - 1);
        float weights = 0;
        float widgetSize;
        bool horizontal = dir == Direction::LEFT || dir == Direction::RIGHT;

        for (Widget* c : m_children)
            weights += c->weight;

        if (dir == Direction::LEFT || dir == Direction::RIGHT)
            widgetSize = (size.x - gapps) / weights;
        else if (dir == Direction::UP || dir == Direction::DOWN)
            widgetSize = (size.y - gapps) / weights;

        // Change widgetSize based on minimum size
        bool done = false;
        while (!done) {
            done = true;
            size_t i = 0;
            for (std::list<Widget*>::iterator it = smallerWidgets.begin(); it != smallerWidgets.end();) {
                Widget* w = *it;
                float currentWidgetSize = (horizontal ? w->minimalSize().x : w->minimalSize().y);
                if (widgetSize * w->weight < currentWidgetSize) {
                    smallerWidgets.erase(it++);
                    i--;
                    weights -= w->weight;
                    widgetSize -= ((currentWidgetSize - widgetSize * w->weight) / weights);
                    done = false;
                } else {
                    it++;
                }
                i++;
            }
        }

        float offset = 0;
        for (Widget* w : m_children) {
            float currentWidgetSize = std::max(widgetSize * w->weight, horizontal ? w->minimalSize().x : w->minimalSize().y);
            switch (dir) {
            case Direction::RIGHT:
                w->arrange(pos + glm::vec2(offset, 0), glm::vec2(currentWidgetSize, size.y));
                break;
            case Direction::LEFT:
                w->arrange(pos + glm::vec2(size.x - currentWidgetSize - offset, 0), glm::vec2(currentWidgetSize, size.y));
                break;
            case Direction::DOWN:
                w->arrange(pos + glm::vec2(0, size.y - currentWidgetSize - offset), glm::vec2(size.x, currentWidgetSize));
                break;
            case Direction::UP:
                w->arrange(pos + glm::vec2(0, offset), glm::vec2(size.x, currentWidgetSize));
                break;
            }
            offset += currentWidgetSize + gap;
        }
    }

    glm::vec2 DirectionalLayout::minimalSize() {
        glm::vec2 minsize(0);
        switch (dir) {
        case Direction::RIGHT:
        case Direction::LEFT:
            for (Widget* child : m_children) {
                glm::vec2 csize = child->minimalSize();
                minsize.x += csize.x;
                if (csize.y > minsize.y)
                    minsize.y = csize.y;
            }
            minsize.x += gap * (m_children.size() - 1);
            break;
        case Direction::DOWN:
        case Direction::UP:
            for (Widget* child : m_children) {
                glm::vec2 csize = child->minimalSize();
                if (csize.x > minsize.x)
                    minsize.x = csize.x;
                minsize.y += csize.y;
            }
            minsize.y += gap * (m_children.size() - 1);
            break;
        }
        glm::vec2 size = this->Widget::minimalSize();
        return glm::max(minsize, size);
    }

}
