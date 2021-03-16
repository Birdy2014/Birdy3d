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

}
