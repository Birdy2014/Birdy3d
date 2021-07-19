#pragma once

#include "render/Light.hpp"

namespace Birdy3d {

    class Spotlight : public Light {
    public:
        Spotlight(glm::vec3 ambient, glm::vec3 diffuse, float innerCutOff, float outerCutOff, float linear, float quadratic, bool shadow_enabled = true);
        void setupShadowMap() override;
        void genShadowMap() override;
        void use(const Shader& lightShader, int id, int textureid) override;

    private:
        float m_far = 25.0f;
        glm::mat4 m_lightSpaceMatrix;
        float m_innerCutOff;
        float m_outerCutOff;
    };

}
