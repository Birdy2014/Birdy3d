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
        bool preserve_child_size;

        DirectionalLayout(Direction dir, Placement placement, UIVector size = UIVector(0), float gap = 0, bool preserve_child_size = false);
        void arrange(glm::vec2 pos, glm::vec2 size) override;
        void arrange_full_size();
        void arrange_preserve_size();
        glm::vec2 minimalSize() override;
    };

}
