#include "ui/DirectionalLayout.hpp"

DirectionalLayout::DirectionalLayout(Direction dir) : dir(dir) {}

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
                offset += widgetSize.x;
                break;
            case Direction::LEFT:
                m = glm::translate(move, glm::vec3(size.x - widgetSize.x - offset, 0, 0));
                offset += widgetSize.x;
                break;
            case Direction::DOWN:
                m = glm::translate(move, glm::vec3(0, size.y - widgetSize.y - offset, 0));
                offset += widgetSize.y;
                break;
            case Direction::UP:
                m = glm::translate(move, glm::vec3(0, offset, 0));
                offset += widgetSize.y;
                break;
        }
        w->arrange(m, widgetSize);
    }
}
