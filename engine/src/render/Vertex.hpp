#ifndef BIRDY3D_VERTEX_HPP
#define BIRDY3D_VERTEX_HPP

#include <glm/glm.hpp>

struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoords;
    glm::vec3 tangent;
};

#endif
