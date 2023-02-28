#pragma once

#include "ecs/Entity.hpp"
#include "events/Event.hpp"
#include "physics/Collider.hpp"

namespace Birdy3d::events {

    class CollisionEvent : public Event {
    public:
        enum Type : int {
            ENTER,
            COLLIDING,
            EXIT
        };

        physics::Collider const* collider_a;
        physics::Collider const* collider_b;
        Type const type;

        CollisionEvent(physics::Collider const* collider_a, physics::Collider const* collider_b, const Type type)
            : collider_a(collider_a)
            , collider_b(collider_b)
            , type(type)
        { }

        bool check_options(std::any options) override
        {
            ecs::Entity* entity = nullptr;
            if (options.type() == typeid(ecs::Entity*))
                entity = std::any_cast<ecs::Entity*>(options);
            else if (options.type() == typeid(std::shared_ptr<ecs::Entity*>))
                entity = std::any_cast<std::shared_ptr<ecs::Entity>>(options).get();
            if (!entity)
                return true;
            auto colliders = entity->get_components<physics::Collider>(false, true);
            if (colliders.empty())
                return false;
            if (!collider_a || !collider_b)
                return false;
            return std::find_if(colliders.begin(), colliders.end(), [&](auto current) { return current.get() == collider_a; }) != colliders.end()
                || std::find_if(colliders.begin(), colliders.end(), [&](auto current) { return current.get() == collider_b; }) != colliders.end();
        }

        physics::Collider const* other(physics::Collider* current)
        {
            if (!collider_a || !collider_b)
                return {};
            if (collider_a == current)
                return collider_b;
            if (collider_b == current)
                return collider_a;
            return {};
        }
    };

}
