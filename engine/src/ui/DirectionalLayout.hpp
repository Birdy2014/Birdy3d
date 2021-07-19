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

        DirectionalLayout() = delete;
        DirectionalLayout(Direction dir, float gap = 0, bool preserve_child_size = false);
        void arrange(const std::list<std::unique_ptr<Widget>>& children, glm::vec2 pos, glm::vec2 size) const override;
        glm::vec2 minimal_size(const std::list<std::unique_ptr<Widget>>& children) const override;

    private:
        void arrange_full_size(const std::list<std::unique_ptr<Widget>>& children, glm::vec2 pos, glm::vec2 size) const;
        void arrange_preserve_size(const std::list<std::unique_ptr<Widget>>& children, glm::vec2 pos, glm::vec2 size) const;
    };

}
