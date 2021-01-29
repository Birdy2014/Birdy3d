#include "physics/CollisionMesh.hpp"

#include "core/GameObject.hpp"

#include <glm/gtx/string_cast.hpp>

CollisionMesh::CollisionMesh(std::vector<glm::vec3> vertices) : vertices(vertices) {}

glm::vec3 CollisionMesh::findFurthestPoint(glm::vec3 direction) {
    // TODO: optimize performance
    glm::mat4 transform = this->collider->object->transform.matrix();
    glm::vec3 absPos = this->collider->object->transform.worldPosition();
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
    return furthestVertex;
}
