#pragma once

#include "ui/Layout.hpp"

namespace Birdy3d::ui {

    class GridLayout : public Layout {
    public:
        GridLayout(int gap = 0);
        void arrange(const std::list<std::shared_ptr<Widget>>& children, glm::vec2 pos, glm::vec2 size) const override;
        glm::vec2 minimal_size(const std::list<std::shared_ptr<Widget>>& children) const override;

    private:
        int m_gap;
    };

}
