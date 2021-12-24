#include "ui/DirectionalLayout.hpp"

#include "ui/Widget.hpp"

namespace Birdy3d::ui {

    DirectionalLayout::DirectionalLayout(Direction dir, float gap, bool preserve_child_size)
        : dir(dir)
        , gap(gap)
        , preserve_child_size(preserve_child_size) { }

    void DirectionalLayout::arrange(const std::list<std::shared_ptr<Widget>>& children, glm::vec2 pos, glm::vec2 size) const {
        if (preserve_child_size)
            arrange_preserve_size(children, pos, size);
        else
            arrange_full_size(children, pos, size);
    }

    glm::vec2 DirectionalLayout::minimal_size(const std::list<std::shared_ptr<Widget>>& children) const {
        if (children.empty())
            return glm::vec2(0);
        glm::vec2 minsize(0);
        switch (dir) {
        case Direction::RIGHT:
        case Direction::LEFT:
            for (const auto& child : children) {
                glm::vec2 csize = child->minimal_size();
                minsize.x += csize.x;
                if (csize.y > minsize.y)
                    minsize.y = csize.y;
            }
            minsize.x += gap * (children.size() - 1);
            break;
        case Direction::DOWN:
        case Direction::UP:
            for (const auto& child : children) {
                glm::vec2 csize = child->minimal_size();
                if (csize.x > minsize.x)
                    minsize.x = csize.x;
                minsize.y += csize.y;
            }
            minsize.y += gap * (children.size() - 1);
            break;
        }
        return minsize;
    }

    void DirectionalLayout::arrange_full_size(const std::list<std::shared_ptr<Widget>>& children, glm::vec2 pos, glm::vec2 size) const {
        std::list<Widget*> smaller_widgets;
        for (const auto& widget : children)
            smaller_widgets.push_back(widget.get());
        float gapps = gap * (children.size() - 1);
        float weights = 0;
        float widget_size;
        bool horizontal = dir == Direction::LEFT || dir == Direction::RIGHT;

        for (const auto& c : children)
            weights += c->options.weight;

        if (dir == Direction::LEFT || dir == Direction::RIGHT)
            widget_size = (size.x - gapps) / weights;
        else if (dir == Direction::UP || dir == Direction::DOWN)
            widget_size = (size.y - gapps) / weights;

        // Change widgetSize based on minimum size
        bool done = false;
        while (!done) {
            done = true;
            size_t i = 0;
            for (std::list<Widget*>::iterator it = smaller_widgets.begin(); it != smaller_widgets.end();) {
                Widget* w = *it;
                float current_widget_size = (horizontal ? w->minimal_size().x : w->minimal_size().y);
                if (widget_size * w->options.weight < current_widget_size) {
                    it = smaller_widgets.erase(it);
                    i--;
                    weights -= w->options.weight;
                    widget_size -= ((current_widget_size - widget_size * w->options.weight) / weights);
                    done = false;
                } else {
                    it++;
                }
                i++;
            }
        }

        float offset = 0;
        for (const auto& w : children) {
            float current_widget_size = std::max(widget_size * w->options.weight, horizontal ? w->minimal_size().x : w->minimal_size().y);
            switch (dir) {
            case Direction::RIGHT:
                w->arrange(pos + glm::vec2(offset, 0), glm::vec2(current_widget_size, size.y));
                break;
            case Direction::LEFT:
                w->arrange(pos + glm::vec2(size.x - current_widget_size - offset, 0), glm::vec2(current_widget_size, size.y));
                break;
            case Direction::DOWN:
                w->arrange(pos + glm::vec2(0, size.y - current_widget_size - offset), glm::vec2(size.x, current_widget_size));
                break;
            case Direction::UP:
                w->arrange(pos + glm::vec2(0, offset), glm::vec2(size.x, current_widget_size));
                break;
            }
            offset += current_widget_size + gap;
        }
    }

    void DirectionalLayout::arrange_preserve_size(const std::list<std::shared_ptr<Widget>>& children, glm::vec2 pos, glm::vec2 size) const {
        float offset = 0;
        for (const auto& w : children) {
            glm::vec2 widget_size = w->preferred_size(size);
            switch (dir) {
            case Direction::RIGHT:
                w->arrange(pos + glm::vec2(offset, 0), widget_size);
                offset += widget_size.x + gap;
                break;
            case Direction::LEFT:
                w->arrange(pos + glm::vec2(size.x - widget_size.x - offset, 0), widget_size);
                offset += widget_size.x + gap;
                break;
            case Direction::DOWN:
                w->arrange(pos + glm::vec2(0, size.y - widget_size.y - offset), widget_size);
                offset += widget_size.y + gap;
                break;
            case Direction::UP:
                w->arrange(pos + glm::vec2(0, offset), widget_size);
                offset += widget_size.y + gap;
                break;
            }
        }
    }

}
