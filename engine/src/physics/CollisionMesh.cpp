#include "physics/CollisionMesh.hpp"

#include "core/GameObject.hpp"

#include <glm/gtx/string_cast.hpp>

CollisionMesh::CollisionMesh(std::vector<glm::vec3> vertices) : vertices(vertices) {
    pos = glm::vec3(0);
}

glm::vec3 CollisionMesh::findFurthestPoint(glm::vec3 direction) {
    glm::mat4 transform = this->collider->object->absTransform();
    glm::vec3 absPos = transform * glm::vec4(pos, 1);
    float max = std::numeric_limits<float>::min();
    glm::vec3 furthestVertex = glm::vec3(transform * glm::vec4(vertices[0], 1.0f));
    glm::vec3 worldVertex;
    for (int i = 0; i < vertices.size(); i++) {
        worldVertex = glm::vec3(transform * glm::vec4(vertices[i], 1.0f));
        float dot = glm::dot(worldVertex - absPos, direction);
        if (dot > max) {
            max = dot;
            furthestVertex = worldVertex;
        }
    }
    //std::cout << glm::to_string(worldVertex) << std::endl;
    return furthestVertex;
}
