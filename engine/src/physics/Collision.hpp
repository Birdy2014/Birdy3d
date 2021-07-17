#pragma once

#include <glm/glm.hpp>

namespace Birdy3d {

    class Collider;

    class CollisionPoints {
    public:
        glm::vec3 furthestA; // Furthest point of A into B
        glm::vec3 furthestB; // Furthest point of B into A
        glm::vec3 normal; // B – A normalized
        float depth; // Length of B – A
        bool hasCollision = false;
    };

    class Collision {
    public:
        Collider* colliderA;
        Collider* colliderB;
        CollisionPoints points;

        Collision(Collider* colliderA, Collider* colliderB)
            : colliderA(colliderA)
            , colliderB(colliderB) { }

        bool contains(Collider* collider) {
            return collider == colliderA || collider == colliderB;
        }
    };

}
