#pragma once

#include "ui/Widget.hpp"

namespace Birdy3d {

    class ScrollView : public Widget {
    public:
        ScrollView(UIVector pos, UIVector size, Placement placement = Placement::BOTTOM_LEFT, std::string name = "");
        glm::vec2 minimal_size() override;
        void arrange(glm::vec2 pos, glm::vec2 size) override;

    private:
        glm::vec2 m_content_size;
        glm::vec2 m_max_scroll_offset;
        glm::vec2 m_scroll_offset = glm::vec2(0);
        Rectangle* m_scrollbar_background;
        Rectangle* m_scrollbar_bar;
        bool m_scrollbar_grabbed = false;

        void on_scroll(const InputScrollEvent& event) override;
        void on_click(const InputClickEvent& event) override;
        void on_update() override;
        void check_scroll_bounds();
    };

}
