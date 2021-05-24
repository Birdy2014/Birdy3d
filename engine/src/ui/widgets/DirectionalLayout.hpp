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

        DirectionalLayout(Direction dir, Placement placement, UIVector size = UIVector(0), float gap = 0);
        void arrange(glm::vec2 pos, glm::vec2 size) override;
        glm::vec2 minimalSize() override;
    };

}
