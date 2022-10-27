#pragma once

#include "ecs/Forward.hpp"
#include "events/Event.hpp"

namespace Birdy3d::events {

    class TransformChangedEvent : public Event {
    public:
        ecs::Entity* const entity;

        TransformChangedEvent(ecs::Entity* entity)
            : entity(entity)
        { }

        bool check_options(std::any options) override
        {
            return options.type() == typeid(ecs::Entity*) && std::any_cast<ecs::Entity*>(options) == entity;
        }
    };

}
