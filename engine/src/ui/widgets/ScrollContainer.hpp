#pragma once

#include "ui/Container.hpp"

namespace Birdy3d::ui {

    class ScrollContainer : public Container {
    public:
        ScrollContainer(Options);
        glm::vec2 minimal_size() override;
        void arrange(glm::vec2 pos, glm::vec2 size) override;

    private:
        glm::vec2 m_content_size;
        glm::vec2 m_max_scroll_offset;
        glm::vec2 m_scroll_offset = glm::vec2(0);
        Rectangle* m_scrollbar_vertical;
        Rectangle* m_scrollbar_horizontal;
        bool m_scrollbar_vertical_grabbed = false;
        bool m_scrollbar_horizontal_grabbed = false;

        void on_scroll(const events::InputScrollEvent& event) override;
        void on_click(const events::InputClickEvent& event) override;
        void on_update() override;
        void check_scroll_bounds();
    };

}
