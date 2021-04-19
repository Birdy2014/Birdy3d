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

        Textarea(UIVector pos, UIVector size, Placement placement);
        ~Textarea();
        void append(const std::string& text);
        void arrange(glm::mat4 move, glm::vec2 size) override;
        void draw() override;

    private:
        float tmpscroll;
        std::string text;
        std::vector<std::string> lines;
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
        void onClick(InputClickEvent* event);
        void onScroll(InputScrollEvent* event);
        void onChar(InputCharEvent* event);
        void onKey(InputKeyEvent* event);
        void clearSelection();
        void updateCursorEnd();
        glm::ivec3 cursorCharPos();
    };

}
