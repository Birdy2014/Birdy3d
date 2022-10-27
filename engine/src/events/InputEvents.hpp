#pragma once

#include "events/Event.hpp"

namespace Birdy3d::events {

    class InputScrollEvent : public Event {
    public:
        double const xoffset;
        double const yoffset;

        InputScrollEvent(double xoffset, double yoffset)
            : xoffset(xoffset)
            , yoffset(yoffset)
        { }
    };

    class InputClickEvent : public Event {
    public:
        int const button;
        int const action;
        int const mods;

        InputClickEvent(int button, int action, int mods)
            : button(button)
            , action(action)
            , mods(mods)
        { }
    };

    class InputKeyEvent : public Event {
    public:
        int const key;
        int const scancode;
        int const action;
        int const mods;

        InputKeyEvent(int key, int scancode, int action, int mods)
            : key(key)
            , scancode(scancode)
            , action(action)
            , mods(mods)
        { }

        bool check_options(std::any options) override
        {
            int key_option = std::any_cast<int>(options);
            return key_option == key && action == 1; // GLFW_PRESS
        }
    };

    class InputCharEvent : public Event {
    public:
        unsigned int const codepoint;

        InputCharEvent(unsigned int codepoint)
            : codepoint(codepoint)
        { }
    };

}
