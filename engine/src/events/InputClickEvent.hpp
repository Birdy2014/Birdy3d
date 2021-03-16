#pragma once

#include "events/Event.hpp"

namespace Birdy3d {

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

}
