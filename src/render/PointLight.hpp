#ifndef BIRDY3D_POINTLIGHT_HPP
#define BIRDY3D_POINTLIGHT_HPP

#include "Light.hpp"

class PointLight : public Light {
public:
    PointLight(GameObject *o, Shader *depthShader, Shader *lightShader, glm::vec3 ambient, glm::vec3 diffuse, float linear, float quadratic) : Light(o, depthShader, lightShader) {
        this->type = 1;
        this->ambient = ambient;
        this->diffuse = diffuse;
        this->linear = linear;
        this->quadratic = quadratic;
    }

    void setupShadowMap() override;
    void genShadowMap(int id, int textureid) override;
};

#endif