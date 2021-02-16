#pragma once

#include "physics/CollisionShape.hpp"
#include <vector>

namespace Birdy3d {

    class CollisionMesh : public CollisionShape {
    public:
        CollisionMesh(std::vector<glm::vec3> vertices);

    private:
        std::vector<glm::vec3> vertices;

        glm::vec3 findFurthestPoint(glm::vec3 direction) override;
    };

}
