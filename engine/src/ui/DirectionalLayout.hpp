#pragma once

#include "ui/Layout.hpp"

namespace Birdy3d::ui {

    class DirectionalLayout : public Layout {
    public:
        enum class Direction {
            RIGHT,
            LEFT,
            DOWN,
            UP
        };

        Direction dir;
        int gap;
        bool preserve_child_size;

        DirectionalLayout() = delete;
        DirectionalLayout(Direction dir, int gap = 0, bool preserve_child_size = false);
        void arrange(std::list<std::shared_ptr<Widget>> const& children, glm::ivec2 pos, glm::ivec2 size) const override;
        [[nodiscard]] glm::ivec2 minimal_size(std::list<std::shared_ptr<Widget>> const& children) const override;

    private:
        void arrange_full_size(std::list<std::shared_ptr<Widget>> const& children, glm::ivec2 pos, glm::ivec2 size) const;
        void arrange_preserve_size(std::list<std::shared_ptr<Widget>> const& children, glm::ivec2 pos, glm::ivec2 size) const;
    };

}
