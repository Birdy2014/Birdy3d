#pragma once

#include "core/Base.hpp"
#include "scene/Component.hpp"

namespace Birdy3d {

    class Shader;

    class Light : public Component {
    public:
        bool shadow_enabled;
        glm::vec3 ambient;
        glm::vec3 diffuse;
        float linear;
        float quadratic;

        Light(std::shared_ptr<Shader> depthShader, glm::vec3 ambient, glm::vec3 diffuse, float linear, float quadratic, bool shadow_enabled);
        virtual void use(const Shader& lightShader, int id, int textureid) = 0;
        virtual void setupShadowMap() = 0;
        virtual void genShadowMap() = 0;
        void start() override;
        void update() override;
        void cleanup() override;

    protected:
        std::shared_ptr<Shader> m_depthShader;
        unsigned int m_depthMapFBO, m_depthMap;
        const unsigned int SHADOW_WIDTH = 2048, SHADOW_HEIGHT = 2048;
        bool m_shadowMapUpdated = false;
    };

}
