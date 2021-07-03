#pragma once

#include "ui/Widget.hpp"

namespace Birdy3d {

    class InputClickEvent;
    class InputScrollEvent;
    class InputCharEvent;
    class InputKeyEvent;

    class Textarea : public Widget { // TODO: Inherit from TextField
    public:
        float scrollpos;
        bool readonly;

        Textarea(UIVector pos, UIVector size, Placement placement, bool readonly = false);
        void append(const std::string& text);
        void arrange(glm::vec2 pos, glm::vec2 size) override;
        void draw() override;

    protected:
        void on_click(InputClickEvent* event) override;
        void on_scroll(InputScrollEvent* event) override;
        void on_char(InputCharEvent* event) override;
        void on_key(InputKeyEvent* event) override;
        void on_mouse_enter() override;
        void on_mouse_leave() override;

    private:
        float m_tmpscroll;
        std::u32string m_text;
        std::vector<std::u32string> m_lines;
        bool m_selecting = false;
        int m_textCursor = -1;
        int m_textCursorX = -1;
        int m_textCursorY = -1;
        int m_selectionStart = -1;
        int m_selectionStartX = -1;
        int m_selectionStartY = -1;
        int m_selectionEnd = -1;
        int m_selectionEndX = -1;
        int m_selectionEndY = -1;

        void updateLines();
        void clearSelection();
        void updateCursorEnd();
        glm::ivec3 cursorCharPos();
    };

}
