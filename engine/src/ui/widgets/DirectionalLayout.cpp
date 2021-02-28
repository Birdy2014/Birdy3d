#include "ui/widgets/DirectionalLayout.hpp"

namespace Birdy3d {

    DirectionalLayout::DirectionalLayout(Direction dir, Placement placement, UIVector size, float gap)
        : Layout(UIVector(0), size, placement)
        , dir(dir)
        , gap(gap) {
    }

    void DirectionalLayout::arrange(glm::mat4 move, glm::vec2 size) {
        // Initial values
        std::vector<Widget*> smallerWidgets = children;
        float widgetSize;
        float gapps = gap * (children.size() - 1);
        switch (dir) {
        case Direction::RIGHT:
        case Direction::LEFT:
            widgetSize = (size.x - gapps) / children.size();
            break;
        case Direction::DOWN:
        case Direction::UP:
            widgetSize = (size.y - gapps) / children.size();
            break;
        }

        // Determine widgetSize
        bool done = false;
        while (!done) {
            done = true;
            for (size_t i = 0; i < smallerWidgets.size(); i++) {
                Widget* w = smallerWidgets[i];
                switch (dir) {
                case Direction::LEFT:
                case Direction::RIGHT: {
                    float width = w->minimalSize().x;
                    if (widgetSize < width) {
                        smallerWidgets.erase(smallerWidgets.begin() + i);
                        i--;
                        widgetSize -= (width - widgetSize) / smallerWidgets.size();
                        done = false;
                    }
                    break;
                }
                case Direction::DOWN:
                case Direction::UP: {
                    float height = w->minimalSize().y;
                    if (widgetSize < height) {
                        smallerWidgets.erase(smallerWidgets.begin() + i);
                        i--;
                        widgetSize -= (height - widgetSize) / smallerWidgets.size();
                        done = false;
                    }
                    break;
                }
                }
            }
        }

        // Arrange widgets
        float offset = 0;
        float width, height;
        for (Widget* w : children) {
            glm::mat4 m;
            switch (dir) {
            case Direction::RIGHT:
                width = std::max(widgetSize, w->minimalSize().x);
                m = glm::translate(move, glm::vec3(offset, 0, 0));
                w->arrange(m, glm::vec2(width, size.y));
                offset += width;
                break;
            case Direction::LEFT:
                width = std::max(widgetSize, w->minimalSize().x);
                m = glm::translate(move, glm::vec3(size.x - width - offset, 0, 0));
                w->arrange(m, glm::vec2(width, size.y));
                offset += width;
                break;
            case Direction::DOWN:
                height = std::max(widgetSize, w->minimalSize().y);
                m = glm::translate(move, glm::vec3(0, size.y - height - offset, 0));
                w->arrange(m, glm::vec2(size.x, height));
                offset += height;
                break;
            case Direction::UP:
                height = std::max(widgetSize, w->minimalSize().y);
                m = glm::translate(move, glm::vec3(0, offset, 0));
                w->arrange(m, glm::vec2(size.x, height));
                offset += height;
                break;
            }
            offset += gap;
        }
    }

    glm::vec2 DirectionalLayout::minimalSize() {
        glm::vec2 minsize(0);
        switch (dir) {
        case Direction::RIGHT:
        case Direction::LEFT:
            for (Widget* child : children) {
                glm::vec2 csize = child->minimalSize();
                minsize.x += csize.x;
                if (csize.y > minsize.y)
                    minsize.y = csize.y;
            }
            minsize.x += gap * (children.size() - 1);
            break;
        case Direction::DOWN:
        case Direction::UP:
            for (Widget* child : children) {
                glm::vec2 csize = child->minimalSize();
                if (csize.x > minsize.x)
                    minsize.x = csize.x;
                minsize.y += csize.y;
            }
            minsize.y += gap * (children.size() - 1);
            break;
        }
        glm::vec2 size = this->Widget::minimalSize();
        return glm::max(minsize, size);
    }

}
