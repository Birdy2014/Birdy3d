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
        const int button;
        const int action;
        const int mods;

        InputClickEvent(int button, int action, int mods)
            : button(button)
            , action(action)
            , mods(mods) { }
    };

    class InputKeyEvent : public Event {
    public:
        const int key;
        const int scancode;
        const int action;
        const int mods;

        InputKeyEvent(int key, int scancode, int action, int mods)
            : key(key)
            , scancode(scancode)
            , action(action)
            , mods(mods) { }

        bool checkOptions(std::any options) override {
            int key_option = std::any_cast<int>(options);
            return key_option == key && action == 1; // GLFW_PRESS
        }
    };

    class InputCharEvent : public Event {
    public:
        const unsigned int codepoint;

        InputCharEvent(unsigned int codepoint)
            : codepoint(codepoint) { }
    };

}
