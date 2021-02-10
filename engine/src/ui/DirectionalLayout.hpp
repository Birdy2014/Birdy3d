#ifndef BIRDY3D_DIRECTIONALLAYOUT_HPP
#define BIRDY3D_DIRECTIONALLAYOUT_HPP

#include "ui/Widget.hpp"

class DirectionalLayout : public Widget {
public:
    enum class Direction {
        RIGHT,
        LEFT,
        DOWN,
        UP
    };

    Direction dir;

    DirectionalLayout(Direction dir);
    void arrange(glm::mat4 move, glm::vec2 size) override;
};

#endif
