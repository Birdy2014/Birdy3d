#pragma once

#include "core/Component.hpp"
#include <glm/glm.hpp>

namespace Birdy3d {

    class Shader;

    class Light : public Component {
    public:
        Light(Shader* depthShader, glm::vec3 ambient, glm::vec3 diffuse, float linear, float quadratic, float innerCutOff, float outerCutOff);
        Light();
        virtual void use(Shader* lightShader, int id, int textureid) {};
        virtual void setupShadowMap() { }
        virtual void genShadowMap(Shader* lightShader, int id, int textureid) { }
        void start() override;
        void update(float deltaTime) override;
        void cleanup() override { }

    protected:
        Shader* depthShader;
        glm::vec3 ambient;
        glm::vec3 diffuse;
        float linear;
        float quadratic;
        float innerCutOff;
        float outerCutOff;
        unsigned int depthMapFBO, depthMap;
        const unsigned int SHADOW_WIDTH = 2048, SHADOW_HEIGHT = 2048;
        bool shadowMapUpdated = false;
    };

}
