#pragma once

#include "render/Light.hpp"

namespace Birdy3d {

    class PointLight : public Light {
    public:
        PointLight(glm::vec3 ambient, glm::vec3 diffuse, float linear, float quadratic);
        void setupShadowMap() override;
        void genShadowMap() override;
        void use(Shader* lightShader, int id, int textureid) override;

    private:
        float far = 25.0f;
    };

}
