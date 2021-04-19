#pragma once

#include "events/Event.hpp"

namespace Birdy3d {

    class Collider;

    class CollisionEvent : public Event {
    public:
        Collider* colliderA;
        Collider* colliderB;

        CollisionEvent(Collider* colliderA, Collider* colliderB)
            : colliderA(colliderA)
            , colliderB(colliderB) { }

        bool forObject(GameObject* object) override {
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
