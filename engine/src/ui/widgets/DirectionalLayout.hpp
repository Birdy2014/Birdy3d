#pragma once

#include "ui/Layout.hpp"

namespace Birdy3d {

    class DirectionalLayout : public Layout {
    public:
        enum class Direction {
            RIGHT,
            LEFT,
            DOWN,
            UP
        };

        Direction dir;
        float gap;

        DirectionalLayout(Direction dir, Placement placement, glm::vec2 size = glm::vec2(0), Unit unit = Unit::PERCENT, float gap = 0);
        void arrange(glm::mat4 move, glm::vec2 size) override;
        glm::vec2 minimalSize() override;
    };

}
