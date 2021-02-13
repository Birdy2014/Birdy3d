#include "ui/widgets/DirectionalLayout.hpp"
#include <glm/gtx/string_cast.hpp>

DirectionalLayout::DirectionalLayout(Direction dir, Placement placement, glm::vec2 size, Unit unit) : Widget(glm::vec2(0), size, placement, unit), dir(dir) {
    unit = Unit::PIXELS;
}

void DirectionalLayout::arrange(glm::mat4 move, glm::vec2 size) {
    glm::vec2 widgetSize;
    float offset = 0;
    switch (dir) {
        case Direction::RIGHT:
        case Direction::LEFT:
            widgetSize = glm::vec2(size.x / children.size(), size.y);
            break;
        case Direction::DOWN:
        case Direction::UP:
            widgetSize = glm::vec2(size.x, size.y / children.size());
            break;
    }
    for (Widget *w : children) {
        glm::mat4 m;
        switch (dir) {
            case Direction::RIGHT:
                m = glm::translate(move, glm::vec3(offset, 0, 0));
                offset += std::max(widgetSize.x, w->pixelSize(size).x);
                break;
            case Direction::LEFT:
                m = glm::translate(move, glm::vec3(size.x - widgetSize.x - offset, 0, 0));
                offset += std::max(widgetSize.x, w->pixelSize(size).x);
                break;
            case Direction::DOWN:
                m = glm::translate(move, glm::vec3(0, size.y - widgetSize.y - offset, 0));
                offset += std::max(widgetSize.y, w->pixelSize(size).y);
                break;
            case Direction::UP:
                m = glm::translate(move, glm::vec3(0, offset, 0));
                offset += std::max(widgetSize.y, w->pixelSize(size).y);
                break;
        }
        w->arrange(m, widgetSize);
    }
}

glm::vec2 DirectionalLayout::pixelSize(glm::vec2 parentSize) {
    glm::vec2 minsize(0);
    switch (dir) {
        case Direction::RIGHT:
        case Direction::LEFT:
            for (Widget *child : children) {
                glm::vec2 csize = child->size;
                minsize.x += csize.x;
                if (csize.y > minsize.y)
                    minsize.y = csize.y;
            }
            break;
        case Direction::DOWN:
        case Direction::UP:
            for (Widget *child : children) {
                glm::vec2 csize = child->size;
                if (csize.x > minsize.x)
                    minsize.x = csize.x;
                minsize.y += csize.y;
            }
            break;
    }
    glm::vec2 size = this->Widget::pixelSize(parentSize);
    return glm::max(minsize, size);
}
