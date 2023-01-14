#include "physics/CollisionSphere.hpp"

namespace Birdy3d::physics {

    CollisionSphere::CollisionSphere(float radius)
        : m_radius{radius}
    { }

    glm::vec3 CollisionSphere::find_furthest_point(const glm::vec3 direction) const
    {
        return m_radius * glm::normalize(direction);
    }

}
