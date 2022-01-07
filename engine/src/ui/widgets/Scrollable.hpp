#pragma once

#include "ui/Widget.hpp"

#include "ui/Rectangle.hpp"

namespace Birdy3d::ui {

    class Scrollable : public Widget {
    public:
        Scrollable(Options);
        glm::vec2 minimal_size() override;
        void arrange(glm::vec2 pos, glm::vec2 size) override;

    protected:
        glm::vec2 m_scroll_offset = glm::vec2(0);
        glm::vec2 m_content_size;
        glm::vec2 m_max_scroll_offset;

        void draw() override;
        void on_scroll(const events::InputScrollEvent& event) override;
        void on_click(const events::InputClickEvent& event) override;
        void on_update() override;

    private:
        Rectangle m_scrollbar_vertical;
        Rectangle m_scrollbar_horizontal;
        bool m_scrollbar_vertical_grabbed = false;
        bool m_scrollbar_horizontal_grabbed = false;

        void check_scroll_bounds();
    };

}
