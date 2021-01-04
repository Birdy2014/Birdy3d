#ifndef BIRDY3D_POINTLIGHT_HPP
#define BIRDY3D_POINTLIGHT_HPP

#include "core/RessourceManager.hpp"
#include "render/Light.hpp"

class PointLight : public Light {
public:
    PointLight(glm::vec3 ambient, glm::vec3 diffuse, float linear, float quadratic) : Light() {
        this->depthShader = RessourceManager::getShader("point_light_depth");
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
