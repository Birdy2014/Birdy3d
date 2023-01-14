#pragma once

#include "physics/CollisionShape.hpp"

namespace Birdy3d::physics {

    class CollisionSphere : public CollisionShape {
    public:
        CollisionSphere(float radius);

        [[nodiscard]] virtual glm::vec3 find_furthest_point(const glm::vec3 direction) const override;

    private:
        float m_radius;
    };

}
