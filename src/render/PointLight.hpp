#ifndef BIRDY3D_POINTLIGHT_HPP
#define BIRDY3D_POINTLIGHT_HPP

#include "Light.hpp"

class PointLight : public Light {
public:
    PointLight(Shader *depthShader, glm::vec3 ambient, glm::vec3 diffuse, float linear, float quadratic) : Light(depthShader) {
        this->type = 1;
        this->ambient = ambient;
        this->diffuse = diffuse;
        this->linear = linear;
        this->quadratic = quadratic;
    }

    void setupShadowMap() override;
    void genShadowMap(Shader *lightShader, int id, int textureid) override;
};

#endif