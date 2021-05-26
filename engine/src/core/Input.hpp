#pragma once

#include "core/Application.hpp"
#include <glm/glm.hpp>

namespace Birdy3d {

    class Input {
    public:
        enum Cursor {
            CURSOR_DEFAULT,
            CURSOR_ARROW,
            CURSOR_MOVE,
            CURSOR_HRESIZE,
            CURSOR_VRESIZE,
            CURSOR_LAST
        };

        static void init();
        static void update();
        static bool keyPressed(int key);
        static glm::vec2 cursorPos();
        static glm::vec2 cursorPosOffset();
        static bool buttonPressed(int button);
        static void setCursorHidden(bool hidden);
        static void toggleCursorHidden();
        static bool isCursorHidden();
        static void setCursor(Cursor cursor);

    private:
        static bool cursorHidden;
        static glm::vec2 currentCursorPos;
        static glm::vec2 lastCursorPos;
        static GLFWcursor* cursors[CURSOR_LAST];
    };

}
