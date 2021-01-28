#ifndef BIRDY3D_COLLISIONSHAPE_HPP
#define BIRDY3D_COLLISIONSHAPE_HPP

#include "physics/Collider.hpp"
#include <glm/glm.hpp>

class CollisionShape {
public:
    Collider *collider;

    virtual glm::vec3 findFurthestPoint(glm::vec3 direction) = 0;

protected:
    glm::vec3 pos;
};

#endif
