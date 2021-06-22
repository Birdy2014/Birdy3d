#pragma once

#include "render/Light.hpp"

namespace Birdy3d {

    class DirectionalLight : public Light {
    public:
        DirectionalLight(glm::vec3 ambient, glm::vec3 diffuse);
        void setupShadowMap() override;
        void genShadowMap() override;
        void use(Shader* lightShader, int id, int textureid) override;

    private:
        glm::mat4 lightSpaceMatrix;
    };

}
