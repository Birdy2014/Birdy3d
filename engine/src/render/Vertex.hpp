#pragma once

#include <glm/glm.hpp>

namespace Birdy3d::render {

    struct Vertex {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec2 tex_coords;
        glm::vec3 tangent;
    };

}
