#include "core/Transform.hpp"

#include <glm/gtc/matrix_transform.hpp>

Transform3d::Transform3d() {}

Transform3d::Transform3d(Transform3d *parentTransform) {
    this->parentTransform = parentTransform;
}

glm::mat4 Transform3d::matrix() {
    if (changed())
        return computeMatrix();
    else
        return _matrix;
}

void Transform3d::setParentTransform(Transform3d *t) {
    parentTransform = t;
}

bool Transform3d::changed(bool updateStatus) {
    if (position != _position || orientation != _orientation || scale != _scale) {
        if (updateStatus) {
            _position = position;
            _orientation = orientation;
            _scale = scale;
        }
        return true;
    }
    return false;
}

glm::mat4 Transform3d::computeMatrix() {
    if (!changed(true))
        return _matrix;
    glm::mat4 m(1);
    if (this->parentTransform != nullptr)
        m = m * this->parentTransform->matrix();
    m = glm::translate(m, this->position);
    m = glm::rotate(m, this->orientation.x, glm::vec3(1, 0, 0));
    m = glm::rotate(m, this->orientation.y, glm::vec3(0, 1, 0));
    m = glm::rotate(m, this->orientation.z, glm::vec3(0, 0, 1));
    m = glm::scale(m, this->scale);
    _matrix = m;
    return m;
}

glm::vec3 Transform3d::worldPosition() {
    return matrix() * glm::vec4(0, 0, 0, 1);
}

glm::vec3 Transform3d::worldOrientation() {
    if (parentTransform == nullptr) {
        return orientation;
    } else {
        return parentTransform->orientation + orientation;
    }
}

glm::vec3 Transform3d::worldScale() {
    if (parentTransform == nullptr) {
        return scale;
    } else {
        return parentTransform->scale * scale;
    }
}
