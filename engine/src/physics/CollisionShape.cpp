#include "physics/CollisionShape.hpp"

#include "core/Component.hpp"
#include "core/GameObject.hpp"

glm::vec3 CollisionShape::findFurthestPointWorldSpace(glm::vec3 direction) {
    glm::mat4 objectToWorld = this->collider->object->absTransform();
    glm::mat4 worldToObject = glm::inverse(objectToWorld);
    glm::vec3 worldPos = objectToWorld * glm::vec4(this->pos, 1.0f);
    glm::vec3 objectDir = glm::normalize(worldToObject * glm::vec4(worldPos + direction, 1.0f));
    return objectToWorld * glm::vec4(findFurthestPoint(objectDir), 1.0f);
}
