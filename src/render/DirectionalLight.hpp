#ifndef BIRDY3D_DIRECTIONALLIGHT_HPP
#define BIRDY3D_DIRECTIONALLIGHT_HPP

#include "Light.hpp"

class DirectionalLight : public Light {
public:
    DirectionalLight(GameObject *o, glm::vec3 direction, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular) : Light(o) {
        this->type = 0;
        this->direction = direction;
        this->ambient = ambient;
        this->diffuse = diffuse;
        this->specular = specular;
    }
};

#endif