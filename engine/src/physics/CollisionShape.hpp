#pragma once

#include <glm/glm.hpp>

namespace Birdy3d {

    class Collider;

    class CollisionShape {
    public:
        Collider* collider;

        virtual glm::vec3 findFurthestPoint(glm::vec3 direction) = 0;
    };

}
