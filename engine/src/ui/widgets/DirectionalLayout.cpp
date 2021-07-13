#include "ui/widgets/DirectionalLayout.hpp"

namespace Birdy3d {

    DirectionalLayout::DirectionalLayout(Direction dir, Placement placement, UIVector size, float gap, bool preserve_child_size)
        : Layout(UIVector(0), size, placement)
        , dir(dir)
        , gap(gap)
        , preserve_child_size(preserve_child_size) { }

    void DirectionalLayout::arrange(glm::vec2 pos, glm::vec2 size) {
        Widget::arrange(pos, size);

        if (preserve_child_size)
            arrange_preserve_size();
        else
            arrange_full_size();
    }

    void DirectionalLayout::arrange_full_size() {
        std::list<Widget*> smallerWidgets;
        for (const std::unique_ptr<Widget>& widget : m_children)
            smallerWidgets.push_back(widget.get());
        float gapps = gap * (m_children.size() - 1);
        float weights = 0;
        float widgetSize;
        bool horizontal = dir == Direction::LEFT || dir == Direction::RIGHT;

        for (const std::unique_ptr<Widget>& c : m_children)
            weights += c->weight;

        if (dir == Direction::LEFT || dir == Direction::RIGHT)
            widgetSize = (actualSize.x - gapps) / weights;
        else if (dir == Direction::UP || dir == Direction::DOWN)
            widgetSize = (actualSize.y - gapps) / weights;

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
        for (const std::unique_ptr<Widget>& w : m_children) {
            float currentWidgetSize = std::max(widgetSize * w->weight, horizontal ? w->minimalSize().x : w->minimalSize().y);
            switch (dir) {
            case Direction::RIGHT:
                w->arrange(actualPos + glm::vec2(offset, 0), glm::vec2(currentWidgetSize, actualSize.y));
                break;
            case Direction::LEFT:
                w->arrange(actualPos + glm::vec2(actualSize.x - currentWidgetSize - offset, 0), glm::vec2(currentWidgetSize, actualSize.y));
                break;
            case Direction::DOWN:
                w->arrange(actualPos + glm::vec2(0, actualSize.y - currentWidgetSize - offset), glm::vec2(actualSize.x, currentWidgetSize));
                break;
            case Direction::UP:
                w->arrange(actualPos + glm::vec2(0, offset), glm::vec2(actualSize.x, currentWidgetSize));
                break;
            }
            offset += currentWidgetSize + gap;
        }
    }

    void DirectionalLayout::arrange_preserve_size() {
        float offset = 0;
        for (const std::unique_ptr<Widget>& w : m_children) {
            glm::vec2 widget_size = w->preferredSize(actualSize);
            switch (dir) {
            case Direction::RIGHT:
                w->arrange(actualPos + glm::vec2(offset, 0), widget_size);
                offset += widget_size.x + gap;
                break;
            case Direction::LEFT:
                w->arrange(actualPos + glm::vec2(actualSize.x - widget_size.x - offset, 0), widget_size);
                offset += widget_size.x + gap;
                break;
            case Direction::DOWN:
                w->arrange(actualPos + glm::vec2(0, actualSize.y - widget_size.y - offset), widget_size);
                offset += widget_size.y + gap;
                break;
            case Direction::UP:
                w->arrange(actualPos + glm::vec2(0, offset), widget_size);
                offset += widget_size.y + gap;
                break;
            }
        }
    }

    glm::vec2 DirectionalLayout::minimalSize() {
        glm::vec2 minsize(0);
        switch (dir) {
        case Direction::RIGHT:
        case Direction::LEFT:
            for (const std::unique_ptr<Widget>& child : m_children) {
                glm::vec2 csize = child->minimalSize();
                minsize.x += csize.x;
                if (csize.y > minsize.y)
                    minsize.y = csize.y;
            }
            minsize.x += gap * (m_children.size() - 1);
            break;
        case Direction::DOWN:
        case Direction::UP:
            for (const std::unique_ptr<Widget>& child : m_children) {
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
