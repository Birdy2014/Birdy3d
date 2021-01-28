#ifndef BIRDY3D_COLLISIONMESH_HPP
#define BIRDY3D_COLLISIONMESH_HPP

#include "physics/CollisionShape.hpp"
#include <vector>

class CollisionMesh : public CollisionShape {
public:
    CollisionMesh(std::vector<glm::vec3> vertices);

private:
    std::vector<glm::vec3> vertices;

    glm::vec3 findFurthestPoint(glm::vec3 direction) override;
};

#endif
