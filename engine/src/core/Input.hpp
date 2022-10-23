#pragma once

#include "core/Application.hpp"
#include <glm/glm.hpp>

namespace Birdy3d::core {

    class Input {
    public:
        enum Cursor {
            CURSOR_DEFAULT,
            CURSOR_ARROW,
            CURSOR_HAND,
            CURSOR_TEXT,
            CURSOR_MOVE,
            CURSOR_HRESIZE,
            CURSOR_VRESIZE,
            CURSOR_TOP_LEFT_RESIZE,
            CURSOR_TOP_RIGHT_RESIZE,
            CURSOR_BOTTOM_LEFT_RESIZE,
            CURSOR_BOTTOM_RIGHT_RESIZE,
            CURSOR_LAST
        };

        static void init();
        static void update();
        static bool key_pressed(int key);
        static glm::vec2 cursor_pos();
        static glm::vec2 cursor_pos_offset();
        static glm::ivec2 cursor_pos_int();
        static bool button_pressed(int button);
        static void set_cursor_hidden(bool hidden);
        static void toggle_cursor_hidden();
        static bool is_cursor_hidden();
        static void set_cursor(Cursor cursor);

    private:
        static bool cursor_hidden;
        static glm::vec2 current_cursor_pos;
        static glm::vec2 last_cursor_pos;
        static GLFWcursor* cursors[CURSOR_LAST];
    };

}
