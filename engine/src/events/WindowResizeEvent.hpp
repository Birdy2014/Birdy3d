#pragma once

#include "events/Event.hpp"

namespace Birdy3d::events {

    class WindowResizeEvent : public Event {
    public:
        const int width;
        const int height;

        WindowResizeEvent(int width, int height)
            : width(width)
            , height(height) { }
    };

}
