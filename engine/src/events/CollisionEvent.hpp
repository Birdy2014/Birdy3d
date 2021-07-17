#pragma once

#include "events/Event.hpp"
#include "scene/GameObject.hpp"

namespace Birdy3d {

    class Collider;

    class CollisionEvent : public Event {
    public:
        enum Type : int {
            ENTER,
            COLLIDING,
            EXIT
        };

        Collider* const colliderA;
        Collider* const colliderB;
        const Type type;

        CollisionEvent(Collider* colliderA, Collider* colliderB, Type type)
            : colliderA(colliderA)
            , colliderB(colliderB)
            , type(type) { }

        bool checkOptions(std::any options) override {
            GameObject* object = std::any_cast<GameObject*>(options);
            if (!object)
                return true;
            auto colliders = object->getComponents<Collider>(false, true);
            if (colliders.empty())
                return false;
            return std::find(colliders.begin(), colliders.end(), colliderA) != colliders.end() || std::find(colliders.begin(), colliders.end(), colliderB) != colliders.end();
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
