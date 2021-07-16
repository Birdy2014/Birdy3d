#pragma once

#include "events/Event.hpp"

namespace Birdy3d {

    class Collider;

    class CollisionEvent : public Event {
    public:
        enum Type : int {
            ENTER,
            COLLIDING,
            EXIT
        };

        Collider* colliderA;
        Collider* colliderB;
        Type type;

        CollisionEvent(Collider* colliderA, Collider* colliderB, Type type)
            : colliderA(colliderA)
            , colliderB(colliderB)
            , type(type) { }

        bool checkOptions(std::any options) override {
            GameObject* object = std::any_cast<GameObject*>(options);
            if (!object)
                return true;
            Collider* collider = object->getComponent<Collider>();
            if (!collider)
                return false;
            return colliderA == collider || colliderB == collider;
        }

        Collider* other(Collider* current) {
            if (colliderA == current)
                return colliderB;
            if (colliderB == current)
                return colliderA;
            return nullptr;
        }
    };

}
