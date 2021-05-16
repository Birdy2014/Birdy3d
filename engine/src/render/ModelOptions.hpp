#pragma once

#include <glm/glm.hpp>

namespace Birdy3d {

    struct ModelOptions {
        bool useTexture;
        glm::vec4 color;
        float specular;
        glm::vec3 emissive;

        ModelOptions(bool useTexture = true, glm::vec4 color = glm::vec4(0), float specular = 1, glm::vec3 emissive = glm::vec3(0))
            : useTexture(useTexture)
            , color(color)
            , specular(specular)
            , emissive(emissive) { }
    };

}
