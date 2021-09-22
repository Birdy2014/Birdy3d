#pragma once

#include "events/Event.hpp"

namespace Birdy3d {

    class Entity;

    class TransformChangedEvent : public Event {
    public:
        Entity* const entity;

        TransformChangedEvent(Entity* entity)
            : entity(entity) { }

        bool check_options(std::any options) override {
            return options.type() == typeid(Entity*) && std::any_cast<Entity*>(options) == entity;
        }
    };

}
