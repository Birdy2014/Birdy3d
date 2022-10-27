#include "core/Input.hpp"

namespace Birdy3d::core {

    bool Input::cursor_hidden = false;
    glm::vec2 Input::current_cursor_pos = glm::vec2(0);
    glm::vec2 Input::last_cursor_pos = glm::vec2(0);
    GLFWcursor* Input::cursors[Cursor::CURSOR_LAST];

    void Input::init()
    {
        double x, y;
        glfwGetCursorPos(Application::get_window(), &x, &y);
        Input::last_cursor_pos = glm::vec2(x, y);
        Input::current_cursor_pos = glm::vec2(x, y);
        cursors[Cursor::CURSOR_DEFAULT] = nullptr;
        cursors[Cursor::CURSOR_ARROW] = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
        cursors[Cursor::CURSOR_HAND] = glfwCreateStandardCursor(GLFW_HAND_CURSOR);
        cursors[Cursor::CURSOR_TEXT] = glfwCreateStandardCursor(GLFW_IBEAM_CURSOR);
        cursors[Cursor::CURSOR_MOVE] = glfwCreateStandardCursor(GLFW_RESIZE_ALL_CURSOR);
        cursors[Cursor::CURSOR_HRESIZE] = glfwCreateStandardCursor(GLFW_HRESIZE_CURSOR);
        cursors[Cursor::CURSOR_TOP_LEFT_RESIZE] = glfwCreateStandardCursor(GLFW_RESIZE_NWSE_CURSOR);
        cursors[Cursor::CURSOR_TOP_RIGHT_RESIZE] = glfwCreateStandardCursor(GLFW_RESIZE_NESW_CURSOR);
        cursors[Cursor::CURSOR_BOTTOM_LEFT_RESIZE] = glfwCreateStandardCursor(GLFW_RESIZE_NESW_CURSOR);
        cursors[Cursor::CURSOR_BOTTOM_RIGHT_RESIZE] = glfwCreateStandardCursor(GLFW_RESIZE_NWSE_CURSOR);
        cursors[Cursor::CURSOR_VRESIZE] = glfwCreateStandardCursor(GLFW_VRESIZE_CURSOR);
    }

    void Input::update()
    {
        double x, y;
        glfwGetCursorPos(Application::get_window(), &x, &y);
        last_cursor_pos = Input::current_cursor_pos;
        current_cursor_pos = glm::vec2(x, y);
    }

    bool Input::key_pressed(int key)
    {
        return glfwGetKey(Application::get_window(), key) == GLFW_PRESS;
    }

    glm::vec2 Input::cursor_pos()
    {
        return Input::current_cursor_pos;
    }

    glm::vec2 Input::cursor_pos_offset()
    {
        return Input::current_cursor_pos - last_cursor_pos;
    }

    glm::ivec2 Input::cursor_pos_int()
    {
        return current_cursor_pos;
    }

    bool Input::button_pressed(int button)
    {
        return glfwGetMouseButton(Application::get_window(), button) == GLFW_PRESS;
    }

    void Input::set_cursor_hidden(bool hidden)
    {
        int mode;
        if (hidden)
            mode = GLFW_CURSOR_DISABLED;
        else
            mode = GLFW_CURSOR_NORMAL;

        glfwSetInputMode(Application::get_window(), GLFW_CURSOR, mode);
        Input::cursor_hidden = hidden;
    }

    void Input::toggle_cursor_hidden()
    {
        set_cursor_hidden(!Input::cursor_hidden);
    }

    bool Input::is_cursor_hidden()
    {
        return Input::cursor_hidden;
    }

    void Input::set_cursor(Cursor cursor)
    {
        glfwSetCursor(Application::get_window(), cursors[cursor]);
    }

}
