#pragma once

#include "events/Event.hpp"
#include "scene/Entity.hpp"

namespace Birdy3d {

    class Collider;

    class CollisionEvent : public Event {
    public:
        enum Type : int {
            ENTER,
            COLLIDING,
            EXIT
        };

        const std::weak_ptr<Collider> collider_a;
        const std::weak_ptr<Collider> collider_b;
        const Type type;

        CollisionEvent(const std::weak_ptr<Collider> collider_a, const std::weak_ptr<Collider> collider_b, const Type type)
            : collider_a(collider_a)
            , collider_b(collider_b)
            , type(type) { }

        bool check_options(std::any options) override {
            Entity* entity = nullptr;
            if (options.type() == typeid(Entity*))
                entity = std::any_cast<Entity*>(options);
            else if (options.type() == typeid(std::shared_ptr<Entity*>))
                entity = std::any_cast<std::shared_ptr<Entity>>(options).get();
            if (!entity)
                return true;
            auto colliders = entity->get_components<Collider>(false, true);
            if (colliders.empty())
                return false;
            auto collider_a_ptr = collider_a.lock();
            auto collider_b_ptr = collider_b.lock();
            if (!collider_a_ptr || !collider_b_ptr)
                return false;
            return std::find(colliders.begin(), colliders.end(), collider_a_ptr) != colliders.end() || std::find(colliders.begin(), colliders.end(), collider_b_ptr) != colliders.end();
        }

        std::shared_ptr<Collider> other(Collider* current) {
            auto collider_a_ptr = collider_a.lock();
            auto collider_b_ptr = collider_b.lock();
            if (!collider_a_ptr || !collider_b_ptr)
                return {};
            if (collider_a_ptr.get() == current)
                return collider_b_ptr;
            if (collider_b_ptr.get() == current)
                return collider_a_ptr;
            return {};
        }
    };

}
