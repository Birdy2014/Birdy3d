#pragma once

#include "render/Light.hpp"

namespace Birdy3d::render {

    class PointLight : public Light {
    public:
        glm::vec3 ambient;
        glm::vec3 diffuse;
        float linear;
        float quadratic;

        PointLight(glm::vec3 ambient = glm::vec3(0), glm::vec3 diffuse = glm::vec3(0), float linear = 0, float quadratic = 0, bool shadow_enabled = true);
        void setup_shadow_map() override;
        void gen_shadow_map() override;
        void use(const Shader& lightShader, int id, int textureid) override;
        void serialize(serializer::Adapter&) override;

    private:
        float m_far = 25.0f;

        BIRDY3D_REGISTER_DERIVED_TYPE_DEC(Component, PointLight);
    };

}
