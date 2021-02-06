#ifndef BIRDY3D_TRANSFORM_HPP
#define BIRDY3D_TRANSFORM_HPP

#include <glm/glm.hpp>

class Transform3d {
public:
    glm::vec3 position;
    glm::vec3 orientation;
    glm::vec3 scale;

    Transform3d();
    Transform3d(Transform3d *parentTransform);
    glm::mat4 matrix();
    void setParentTransform(Transform3d *t);
    glm::vec3 worldPosition();
    glm::vec3 worldOrientation();
    glm::vec3 worldScale();

protected:
    glm::mat4 _matrix;
    Transform3d *parentTransform = nullptr;

    bool changed(bool updateStatus = false);
    glm::mat4 computeMatrix();

private:
    // Copies for change detection
    glm::vec3 _position;
    glm::vec3 _orientation;
    glm::vec3 _scale;
    glm::mat4 _parentMatrix;
};

#endif
