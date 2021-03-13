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

        bool has(Collider* collider) {
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
