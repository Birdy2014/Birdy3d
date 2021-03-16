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
    };

    class InputCharEvent : public Event {
    public:
        unsigned int codepoint;

        InputCharEvent(unsigned int codepoint)
            : codepoint(codepoint) { }
    };

}
