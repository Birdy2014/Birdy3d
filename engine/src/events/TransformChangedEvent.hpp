#pragma once

#include "events/Event.hpp"

namespace Birdy3d {

    class GameObject;

    class TransformChangedEvent : public Event {
    public:
        GameObject* const object;

        TransformChangedEvent(GameObject* object)
            : object(object) { }

        bool checkOptions(std::any options) override {
            return options.type() == typeid(GameObject*) && std::any_cast<GameObject*>(options) == object;
        }
    };

}
