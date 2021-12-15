#pragma once

#include "events/Forward.hpp"
#include "ui/widgets/TextField.hpp"

namespace Birdy3d::ui {

    class Textarea : public TextField {
    public:
        float scrollpos;

        Textarea(UIVector pos, UIVector size, Placement placement);
        void draw() override;
        void scroll_down();

    protected:
        void on_update() override;
        void on_click(const events::InputClickEvent& event) override;
        void on_scroll(const events::InputScrollEvent& event) override;
        void on_char(const events::InputCharEvent& event) override;
        void on_key(const events::InputKeyEvent& event) override;
        void on_resize() override;

    private:
        // TODO: Remove this and fix Textarea
        std::u32string m_text;
        int m_cursor_pos = -1;
        int m_selection_start = -1;
        int m_selection_end = -1;

        float m_tmpscroll;
        std::vector<size_t> m_lines;
        bool m_selecting = false;

        void update_lines();
        size_t cursor_char_pos();
        glm::ivec2 get_2d_pos(size_t pos);
    };

}
