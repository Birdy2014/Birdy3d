#pragma once

#include "events/Event.hpp"

namespace Birdy3d {

    class WindowResizeEvent : public Event {
    public:
        int width;
        int height;

        WindowResizeEvent(int width, int height)
            : width(width)
            , height(height) { }
    };

}
