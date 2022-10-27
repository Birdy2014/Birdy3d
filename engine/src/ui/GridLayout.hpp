#pragma once

#include "ui/Layout.hpp"

namespace Birdy3d::ui {

    class StaticGridLayout : public Layout {
    public:
        StaticGridLayout(int gap = 0);
        void arrange(std::list<std::shared_ptr<Widget>> const& children, glm::ivec2 pos, glm::ivec2 size) const override;
        [[nodiscard]] glm::ivec2 minimal_size(std::list<std::shared_ptr<Widget>> const& children) const override;

    private:
        int m_gap;
    };

    class DynamicGridLayout : public Layout {
    public:
        DynamicGridLayout(int gap = 0);
        void arrange(std::list<std::shared_ptr<Widget>> const& children, glm::ivec2 pos, glm::ivec2 size) const override;
        [[nodiscard]] glm::ivec2 minimal_size(std::list<std::shared_ptr<Widget>> const& children) const override;
        [[nodiscard]] glm::ivec2 minimal_size(std::list<std::shared_ptr<Widget>> const& children, float suggested_size, Direction) const override;
        [[nodiscard]] float minimal_size(std::list<std::shared_ptr<Widget>> const& children, Direction) const override;

    private:
        int m_gap;
    };

}
