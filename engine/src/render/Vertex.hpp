#pragma once

#include <glm/glm.hpp>

namespace Birdy3d {

    struct Vertex {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec2 texCoords;
        glm::vec3 tangent;
    };

}
