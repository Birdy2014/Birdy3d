#pragma once

#include "ui/Layout.hpp"

namespace Birdy3d::ui {

    class StaticGridLayout : public Layout {
    public:
        StaticGridLayout(int gap = 0);
        void arrange(const std::list<std::shared_ptr<Widget>>& children, glm::ivec2 pos, glm::ivec2 size) const override;
        glm::ivec2 minimal_size(const std::list<std::shared_ptr<Widget>>& children) const override;

    private:
        int m_gap;
    };

    class DynamicGridLayout : public Layout {
    public:
        DynamicGridLayout(int gap = 0);
        void arrange(const std::list<std::shared_ptr<Widget>>& children, glm::ivec2 pos, glm::ivec2 size) const override;
        glm::ivec2 minimal_size(const std::list<std::shared_ptr<Widget>>& children) const override;
        glm::ivec2 minimal_size(const std::list<std::shared_ptr<Widget>>& children, float suggested_size, Direction) const override;
        float minimal_size(const std::list<std::shared_ptr<Widget>>& children, Direction) const override;

    private:
        int m_gap;
    };

}
