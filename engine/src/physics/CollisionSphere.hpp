#ifndef BIRDY3D_COLLISIONSPHERE_HPP
#define BIRDY3D_COLLISIONSPHERE_HPP

#include "physics/CollisionShape.hpp"

class CollisionSphere : public CollisionShape {
public:
    CollisionSphere(glm::vec3 pos, float radius) : radius(radius) {
        this->pos = pos;
    }

private:
    float radius;

    glm::vec3 findFurthestPoint(glm::vec3 direction) override {
        glm::mat4 transform = this->collider->object->absTransform();
        glm::vec3 absPos = transform * glm::vec4(pos, 1);
        // TODO: transform radius
        return absPos + glm::normalize(direction) * radius;
    }
};

#endif
