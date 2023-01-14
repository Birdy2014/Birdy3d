#pragma once

#include "physics/CollisionShape.hpp"

#include <optional>

namespace Birdy3d::physics {

    class CollisionMesh : public CollisionShape {
    public:
        CollisionMesh(std::vector<glm::vec3> const);

        [[nodiscard]] virtual glm::vec3 find_furthest_point(const glm::vec3 direction) const override;

    private:
        std::vector<glm::vec3> m_vertices;
    };

}
