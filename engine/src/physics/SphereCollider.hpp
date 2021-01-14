#ifndef BIRDY3D_SPHERECOLLIDER_HPP
#define BIRDY3D_SPHERECOLLIDER_HPP

#include "physics/CollisionShape.hpp"

class SphereCollider : public CollisionShape {
public:
    SphereCollider(glm::vec3 pos, float radius) : radius(radius) {
        this->pos = pos;
    }

    glm::vec3 findFurthestPoint(glm::vec3 direction) override {
        return pos + glm::normalize(direction) * radius;
    }

private:
    float radius;
};

#endif
