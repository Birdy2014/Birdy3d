#pragma once

#include "render/Light.hpp"

namespace Birdy3d {

    class PointLight : public Light {
    public:
        PointLight(glm::vec3 ambient, glm::vec3 diffuse, float linear, float quadratic, bool shadow_enabled = true);
        void setupShadowMap() override;
        void genShadowMap() override;
        void use(const Shader& lightShader, int id, int textureid) override;

    private:
        float m_far = 25.0f;
    };

}
