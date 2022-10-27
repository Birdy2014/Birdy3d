#pragma once

#include "events/Event.hpp"

namespace Birdy3d::events {

    class WindowResizeEvent : public Event {
    public:
        int const width;
        int const height;

        WindowResizeEvent(int width, int height)
            : width(width)
            , height(height)
        { }
    };

}
