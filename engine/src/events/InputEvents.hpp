#pragma once

#include "events/Event.hpp"

namespace Birdy3d {

    class InputScrollEvent : public Event {
    public:
        const double xoffset;
        const double yoffset;

        InputScrollEvent(double xoffset, double yoffset)
            : xoffset(xoffset)
            , yoffset(yoffset) { }
    };

    class InputClickEvent : public Event {
    public:
        int button;
        int action;
        int mods;

        InputClickEvent(int button, int action, int mods)
            : button(button)
            , action(action)
            , mods(mods) { }
    };

    class InputKeyEvent : public Event {
    public:
        int key;
        int scancode;
        int action;
        int mods;

        InputKeyEvent(int key, int scancode, int action, int mods)
            : key(key)
            , scancode(scancode)
            , action(action)
            , mods(mods) { }

        bool checkOptions(int options) override {
            return options == key && action == GLFW_PRESS;
        }
    };

    class InputCharEvent : public Event {
    public:
        unsigned int codepoint;

        InputCharEvent(unsigned int codepoint)
            : codepoint(codepoint) { }

        size_t utf8(char* const buffer) {
            if (codepoint <= 0x7F) {
                buffer[0] = codepoint;
                return 1;
            }
            if (codepoint <= 0x7FF) {
                buffer[0] = 0xC0 | (codepoint >> 6); /* 110xxxxx */
                buffer[1] = 0x80 | (codepoint & 0x3F); /* 10xxxxxx */
                return 2;
            }
            if (codepoint <= 0xFFFF) {
                buffer[0] = 0xE0 | (codepoint >> 12); /* 1110xxxx */
                buffer[1] = 0x80 | ((codepoint >> 6) & 0x3F); /* 10xxxxxx */
                buffer[2] = 0x80 | (codepoint & 0x3F); /* 10xxxxxx */
                return 3;
            }
            if (codepoint <= 0x10FFFF) {
                buffer[0] = 0xF0 | (codepoint >> 18); /* 11110xxx */
                buffer[1] = 0x80 | ((codepoint >> 12) & 0x3F); /* 10xxxxxx */
                buffer[2] = 0x80 | ((codepoint >> 6) & 0x3F); /* 10xxxxxx */
                buffer[3] = 0x80 | (codepoint & 0x3F); /* 10xxxxxx */
                return 4;
            }
            return 0;
        }
    };

}
