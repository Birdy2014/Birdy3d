#pragma once

#include "physics/Forward.hpp"
#include <glm/glm.hpp>
#include <optional>

namespace Birdy3d::physics {

    class CollisionPoints {
    public:
        glm::vec3 furthest_a; // Furthest point of A into B
        glm::vec3 furthest_b; // Furthest point of B into A
        glm::vec3 normal; // B – A normalized
        float depth; // Length of B – A
    };

    class Collision {
    public:
        ColliderComponent const& collider_component_a;
        ColliderComponent const& collider_component_b;
        std::optional<CollisionPoints> points;

        Collision(ColliderComponent const& collider_a, ColliderComponent const& collider_b)
            : collider_component_a(collider_a)
            , collider_component_b(collider_b)
        { }

        [[nodiscard]] bool contains(ColliderComponent const& collider_component) const
        {
            return &collider_component == &collider_component_a || &collider_component == &collider_component_b;
        }
    };

}
