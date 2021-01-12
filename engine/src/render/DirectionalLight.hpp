#ifndef BIRDY3D_DIRECTIONALLIGHT_HPP
#define BIRDY3D_DIRECTIONALLIGHT_HPP

#include "core/RessourceManager.hpp"
#include "render/Light.hpp"

class DirectionalLight : public Light {
public:
    DirectionalLight(glm::vec3 ambient, glm::vec3 diffuse) : Light() {
        this->depthShader = RessourceManager::getShader("directional_light_depth");
        this->ambient = ambient;
        this->diffuse = diffuse;
    }

    void setupShadowMap() override;
    void genShadowMap(Shader *lightShader, int id, int textureid) override;
    void use(Shader *lightShader, int id, int textureid) override;

private:
    glm::mat4 lightSpaceMatrix;
};

#endif
