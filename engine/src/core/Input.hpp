#ifndef BIRDY3D_INPUT_HPP
#define BIRDY3D_INPUT_HPP

#include <glm/glm.hpp>

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

#endif
