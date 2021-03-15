#pragma once

#include "ui/Widget.hpp"

namespace Birdy3d {

    class InputClickEvent;
    class InputScrollEvent;

    class Textarea : public Widget {
    public:
        float scrollpos;

        Textarea(UIVector pos, UIVector size, Placement placement);
        void append(const std::string& text);
        void arrange(glm::mat4 move, glm::vec2 size) override;
        void draw() override;

    private:
        float tmpscroll;
        std::string text;
        std::vector<std::string> lines;
        int textCursor = -1;
        int textCursorX = -1;
        int textCursorY = -1;

        std::vector<std::string> getLines();
        void onClick(InputClickEvent* event);
        void onScroll(InputScrollEvent* event);
    };

}
