#pragma once

#include "ui/widgets/TextField.hpp"

namespace Birdy3d {

    class InputClickEvent;
    class InputScrollEvent;
    class InputCharEvent;
    class InputKeyEvent;

    class Textarea : public TextField {
    public:
        float scrollpos;

        Textarea(UIVector pos, UIVector size, Placement placement);
        void draw() override;
        void scroll_down();

    protected:
        void on_update() override;
        void on_click(const InputClickEvent& event) override;
        void on_scroll(const InputScrollEvent& event) override;
        void on_char(const InputCharEvent& event) override;
        void on_key(const InputKeyEvent& event) override;
        void on_resize() override;

    private:
        float m_tmpscroll;
        std::vector<size_t> m_lines;
        bool m_selecting = false;

        void update_lines();
        size_t cursor_char_pos();
        glm::ivec2 get_2d_pos(size_t pos);
    };

}
