#pragma once

#include "ui/Widget.hpp"

namespace Birdy3d {

    class ScrollView : public Widget {
    public:
        using Widget::Widget;
        glm::vec2 minimal_size() override;
        void arrange(glm::vec2 pos, glm::vec2 size) override;

    private:
        glm::vec2 m_content_size;
        glm::vec2 m_max_scroll_offset;
        glm::vec2 m_scroll_offset = glm::vec2(0);

        void on_scroll(const InputScrollEvent& event) override;
    };

}
