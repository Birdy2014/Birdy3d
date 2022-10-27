#pragma once

#include "physics/Forward.hpp"
#include <glm/glm.hpp>

namespace Birdy3d::physics {

    class CollisionPoints {
    public:
        glm::vec3 furthest_a; // Furthest point of A into B
        glm::vec3 furthest_b; // Furthest point of B into A
        glm::vec3 normal; // B – A normalized
        float depth; // Length of B – A
        bool has_collision = false;
    };

    class Collision {
    public:
        Collider* collider_a;
        Collider* collider_b;
        CollisionPoints points;

        Collision(Collider* collider_a, Collider* collider_b)
            : collider_a(collider_a)
            , collider_b(collider_b)
        { }

        bool contains(Collider* collider)
        {
            return collider == collider_a || collider == collider_b;
        }
    };

}
