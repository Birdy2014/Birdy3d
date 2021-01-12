#ifndef BIRDY3D_SPOTLIGHT_HPP
#define BIRDY3D_SPOTLIGHT_HPP

#include "render/Light.hpp"

class Spotlight : public Light {
public:
    Spotlight(glm::vec3 ambient, glm::vec3 diffuse, float innerCutOff, float outerCutOff, float linear, float quadratic);
    void setupShadowMap() override;
    void genShadowMap(Shader *lightShader, int id, int textureid) override;
    void use(Shader *lightShader, int id, int textureid) override;

private:
    float far = 25.0f;
    glm::mat4 lightSpaceMatrix;
    float innerCutOff;
    float outerCutOff;
};

#endif
