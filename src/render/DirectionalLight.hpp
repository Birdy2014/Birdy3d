#ifndef BIRDY3D_DIRECTIONALLIGHT_HPP
#define BIRDY3D_DIRECTIONALLIGHT_HPP

#include "Light.hpp"

class DirectionalLight : public Light {
public:
    DirectionalLight(GameObject *o, Shader *depthShader, Shader *lightShader, glm::vec3 direction, glm::vec3 ambient, glm::vec3 diffuse) : Light(o, depthShader, lightShader) {
        this->type = 0;
        this->direction = direction;
        this->ambient = ambient;
        this->diffuse = diffuse;
    }

    void setupShadowMap() override;
    void genShadowMap(int id, int textureid) override;
};

#endif