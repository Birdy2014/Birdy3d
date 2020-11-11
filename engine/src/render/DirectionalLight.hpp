#ifndef BIRDY3D_DIRECTIONALLIGHT_HPP
#define BIRDY3D_DIRECTIONALLIGHT_HPP

#include "render/Light.hpp"

class DirectionalLight : public Light {
public:
    DirectionalLight(Shader *depthShader, glm::vec3 direction, glm::vec3 ambient, glm::vec3 diffuse) : Light(depthShader) {
        this->type = 0;
        this->direction = direction;
        this->ambient = ambient;
        this->diffuse = diffuse;
    }

    void setupShadowMap() override;
    void genShadowMap(Shader *lightShader, int id, int textureid) override;
};

#endif
