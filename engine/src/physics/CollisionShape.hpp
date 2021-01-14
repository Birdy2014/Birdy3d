#ifndef BIRDY3D_COLLISIONSHAPE_HPP
#define BIRDY3D_COLLISIONSHAPE_HPP

#include "physics/Collider.hpp"
#include <glm/glm.hpp>

class CollisionShape {
public:
    Collider *collider;

    glm::vec3 findFurthestPointWorldSpace(glm::vec3 direction);

protected:
    glm::vec3 pos;

    virtual glm::vec3 findFurthestPoint(glm::vec3 direction) = 0;
};

#endif
