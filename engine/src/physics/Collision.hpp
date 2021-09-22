#pragma once

#include <glm/glm.hpp>

namespace Birdy3d {

    class Collider;

    class CollisionPoints {
    public:
        glm::vec3 furthest_a; // Furthest point of A into B
        glm::vec3 furthest_b; // Furthest point of B into A
        glm::vec3 normal; // B – A normalized
        float depth; // Length of B – A
        bool hasCollision = false;
    };

    class Collision {
    public:
        Collider* collider_a;
        Collider* collider_b;
        CollisionPoints points;

        Collision(Collider* colliderA, Collider* colliderB)
            : collider_a(colliderA)
            , collider_b(colliderB) { }

        bool contains(Collider* collider) {
            return collider == collider_a || collider == collider_b;
        }
    };

}
