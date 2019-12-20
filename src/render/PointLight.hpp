#ifndef BIRDY3D_POINTLIGHT_HPP
#define BIRDY3D_POINTLIGHT_HPP

#include "Light.hpp"

class PointLight : public Light {
public:
    PointLight(glm::vec3 position, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, float linear, float quadratic) {
        this->type = 1;
        this->position = position;
        this->ambient = ambient;
        this->diffuse = diffuse;
        this->specular = specular;
        this->linear = linear;
        this->quadratic = quadratic;
    }
};

#endif