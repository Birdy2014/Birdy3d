#pragma once

#include <glm/glm.hpp>

namespace Birdy3d {

    class Input {
    public:
        static void init();
        static void update();
        static bool keyPressed(int key);
        static glm::vec2 cursorPos();
        static glm::vec2 cursorPosOffset();
        static bool buttonPressed(int button);
        static void setCursorHidden(bool hidden);
        static void toggleCursorHidden();
        static bool isCursorHidden();

    private:
        static bool cursorHidden;
        static glm::vec2 currentCursorPos;
        static glm::vec2 lastCursorPos;
    };

}
