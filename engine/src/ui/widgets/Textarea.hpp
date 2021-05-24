#pragma once

#include "ui/Widget.hpp"

namespace Birdy3d {

    class InputClickEvent;
    class InputScrollEvent;
    class InputCharEvent;
    class InputKeyEvent;

    class Textarea : public Widget {
    public:
        float scrollpos;
        bool readonly;
        TextRenderer* renderer;

        Textarea(UIVector pos, UIVector size, Placement placement, bool readonly = false);
        void append(const std::string& text);
        void arrange(glm::vec2 pos, glm::vec2 size) override;
        void draw() override;

    protected:
        bool onClick(InputClickEvent* event, bool hover) override;
        bool onScroll(InputScrollEvent* event, bool hover) override;
        bool onChar(InputCharEvent* event, bool hover) override;
        bool onKey(InputKeyEvent* event, bool hover) override;

    private:
        float tmpscroll;
        std::u32string text;
        std::vector<std::u32string> lines;
        bool selecting = false;
        int textCursor = -1;
        int textCursorX = -1;
        int textCursorY = -1;
        int selectionStart = -1;
        int selectionStartX = -1;
        int selectionStartY = -1;
        int selectionEnd = -1;
        int selectionEndX = -1;
        int selectionEndY = -1;

        void updateLines();
        void clearSelection();
        void updateCursorEnd();
        glm::ivec3 cursorCharPos();
    };

}
