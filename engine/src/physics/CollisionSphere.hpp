#ifndef BIRDY3D_COLLISIONSPHERE_HPP
#define BIRDY3D_COLLISIONSPHERE_HPP

#include "physics/CollisionShape.hpp"

class CollisionSphere : public CollisionShape {
public:
    CollisionSphere(glm::vec3 pos, float radius) : radius(radius) {
        this->pos = pos;
    }

    glm::vec3 findFurthestPoint(glm::vec3 direction) override {
        glm::mat4 transform = this->collider->object->absTransform();
        glm::vec3 absPos = transform * glm::vec4(pos, 1);
        return absPos + glm::normalize(direction) * absRadius();
    }

private:
    float radius;

    float absRadius() {
        glm::mat4 transform = this->collider->object->absTransform();
        glm::vec3 origin = transform * glm::vec4(0, 0, 0, 1);
        glm::vec3 other = transform * glm::vec4(radius, 0, 0, 1);
        return glm::length(other - origin);
    }
};

#endif
