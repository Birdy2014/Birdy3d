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

        bool forObject(GameObject* object) override {
            Collider* collider = object->getComponent<Collider>();
            if (!collider)
                return false;
            return colliderA == collider || colliderB == collider;
        }

        bool checkOptions(int options) override {
            return options == type || (options == Type::COLLIDING && type == Type::ENTER);
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
