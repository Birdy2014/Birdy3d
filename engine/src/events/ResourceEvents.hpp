#pragma once

#include "events/Event.hpp"

namespace Birdy3d::events {

    class ResourceLoadEvent : public Event {
    public:
        ResourceLoadEvent()
        { }

        bool check_options(std::any) override
        {
            return true;
        }
    };

}
