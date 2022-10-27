#include "ui/DirectionalLayout.hpp"

#include "ui/Widget.hpp"

namespace Birdy3d::ui {

    DirectionalLayout::DirectionalLayout(Direction dir, int gap, bool preserve_child_size)
        : dir(dir)
        , gap(gap)
        , preserve_child_size(preserve_child_size)
    { }

    void DirectionalLayout::arrange(std::list<std::shared_ptr<Widget>> const& children, glm::ivec2 pos, glm::ivec2 size) const
    {
        if (preserve_child_size)
            arrange_preserve_size(children, pos, size);
        else
            arrange_full_size(children, pos, size);
    }

    glm::ivec2 DirectionalLayout::minimal_size(std::list<std::shared_ptr<Widget>> const& children) const
    {
        if (children.empty())
            return glm::ivec2(0);
        glm::ivec2 minsize(0);
        switch (dir) {
        case Direction::RIGHT:
        case Direction::LEFT:
            for (auto const& child : children) {
                glm::ivec2 csize = child->minimal_size();
                minsize.x += csize.x;
                if (csize.y > minsize.y)
                    minsize.y = csize.y;
            }
            minsize.x += gap * (children.size() - 1);
            break;
        case Direction::DOWN:
        case Direction::UP:
            for (auto const& child : children) {
                glm::ivec2 csize = child->minimal_size();
                if (csize.x > minsize.x)
                    minsize.x = csize.x;
                minsize.y += csize.y;
            }
            minsize.y += gap * (children.size() - 1);
            break;
        }
        return minsize;
    }

    void DirectionalLayout::arrange_full_size(std::list<std::shared_ptr<Widget>> const& children, glm::ivec2 pos, glm::ivec2 size) const
    {
        std::list<Widget*> smaller_widgets;
        for (auto const& widget : children)
            smaller_widgets.push_back(widget.get());
        int gapps = gap * (children.size() - 1);
        float weights = 0;
        int widget_size;
        bool horizontal = dir == Direction::LEFT || dir == Direction::RIGHT;

        for (auto const& c : children)
            weights += c->weight;

        if (horizontal)
            widget_size = (size.x - gapps) / weights;
        else
            widget_size = (size.y - gapps) / weights;

        // Change widget_size based on minimum size
        bool done = false;
        while (!done) {
            done = true;
            size_t i = 0;
            for (std::list<Widget*>::iterator it = smaller_widgets.begin(); it != smaller_widgets.end();) {
                Widget* w = *it;
                int current_widget_size = (horizontal ? w->minimal_size().x : w->minimal_size().y);
                if (widget_size * w->weight < current_widget_size) {
                    it = smaller_widgets.erase(it);
                    i--;
                    weights -= w->weight;
                    widget_size -= ((current_widget_size - widget_size * w->weight) / weights);
                    done = false;
                } else {
                    it++;
                }
                i++;
            }
        }

        int offset = 0;
        for (auto const& w : children) {
            int current_widget_size = std::max(static_cast<int>(widget_size * w->weight), horizontal ? w->minimal_size().x : w->minimal_size().y);
            switch (dir) {
            case Direction::RIGHT:
                w->arrange(pos + glm::ivec2(offset, 0), glm::ivec2(current_widget_size, size.y));
                break;
            case Direction::LEFT:
                w->arrange(pos + glm::ivec2(size.x - current_widget_size - offset, 0), glm::ivec2(current_widget_size, size.y));
                break;
            case Direction::DOWN:
                w->arrange(pos + glm::ivec2(0, offset), glm::ivec2(size.x, current_widget_size));
                break;
            case Direction::UP:
                w->arrange(pos + glm::ivec2(0, size.y - current_widget_size - offset), glm::ivec2(size.x, current_widget_size));
                break;
            }
            offset += current_widget_size + gap;
        }
    }

    void DirectionalLayout::arrange_preserve_size(std::list<std::shared_ptr<Widget>> const& children, glm::ivec2 pos, glm::ivec2 size) const
    {
        int offset = 0;
        for (auto const& w : children) {
            glm::ivec2 widget_size = w->preferred_size(size);
            switch (dir) {
            case Direction::RIGHT:
                w->arrange(pos + glm::ivec2(offset, 0), widget_size);
                offset += widget_size.x + gap;
                break;
            case Direction::LEFT:
                w->arrange(pos + glm::ivec2(size.x - widget_size.x - offset, 0), widget_size);
                offset += widget_size.x + gap;
                break;
            case Direction::DOWN:
                w->arrange(pos + glm::ivec2(0, offset), widget_size);
                offset += widget_size.y + gap;
                break;
            case Direction::UP:
                w->arrange(pos + glm::ivec2(0, size.y - widget_size.y - offset), widget_size);
                offset += widget_size.y + gap;
                break;
            }
        }
    }

}
