#include "core/Input.hpp"

namespace Birdy3d {

    bool Input::cursorHidden = false;
    glm::vec2 Input::currentCursorPos = glm::vec2(0);
    glm::vec2 Input::lastCursorPos = glm::vec2(0);
    GLFWcursor* Input::cursors[Cursor::CURSOR_LAST];

    void Input::init() {
        double x, y;
        glfwGetCursorPos(Application::getWindow(), &x, &y);
        Input::lastCursorPos = glm::vec2(x, y);
        Input::currentCursorPos = glm::vec2(x, y);
        cursors[Cursor::CURSOR_DEFAULT] = nullptr;
        cursors[Cursor::CURSOR_ARROW] = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
        cursors[Cursor::CURSOR_HAND] = glfwCreateStandardCursor(GLFW_HAND_CURSOR);
        cursors[Cursor::CURSOR_TEXT] = glfwCreateStandardCursor(GLFW_IBEAM_CURSOR);
        cursors[Cursor::CURSOR_MOVE] = glfwCreateStandardCursor(GLFW_CROSSHAIR_CURSOR);
        cursors[Cursor::CURSOR_HRESIZE] = glfwCreateStandardCursor(GLFW_HRESIZE_CURSOR);
        cursors[Cursor::CURSOR_VRESIZE] = glfwCreateStandardCursor(GLFW_VRESIZE_CURSOR);
    }

    void Input::update() {
        double x, y;
        glfwGetCursorPos(Application::getWindow(), &x, &y);
        Input::lastCursorPos = Input::currentCursorPos;
        Input::currentCursorPos = glm::vec2(x + 2, Application::getViewportSize().y - y);
    }

    bool Input::keyPressed(int key) {
        return glfwGetKey(Application::getWindow(), key) == GLFW_PRESS;
    }

    glm::vec2 Input::cursorPos() {
        return Input::currentCursorPos;
    }

    glm::vec2 Input::cursorPosOffset() {
        return Input::currentCursorPos - Input::lastCursorPos;
    }

    bool Input::buttonPressed(int button) {
        glfwGetMouseButton(Application::getWindow(), button) == GLFW_PRESS;
    }

    void Input::setCursorHidden(bool hidden) {
        int mode;
        if (hidden)
            mode = GLFW_CURSOR_DISABLED;
        else
            mode = GLFW_CURSOR_NORMAL;

        glfwSetInputMode(Application::getWindow(), GLFW_CURSOR, mode);
        Input::cursorHidden = hidden;
    }

    void Input::toggleCursorHidden() {
        setCursorHidden(!Input::cursorHidden);
    }

    bool Input::isCursorHidden() {
        return Input::cursorHidden;
    }

    void Input::setCursor(Cursor cursor) {
        glfwSetCursor(Application::getWindow(), cursors[cursor]);
    }

}
