#pragma once

#include "render/Light.hpp"

namespace Birdy3d::render {

    class DirectionalLight : public Light {
    public:
        glm::vec3 ambient;
        glm::vec3 diffuse;

        DirectionalLight(glm::vec3 ambient = glm::vec3(0), glm::vec3 diffuse = glm::vec3(0), bool shadow_enabled = true);
        void setup_shadow_map() override;
        void gen_shadow_map() override;
        void use(const Shader& lightShader, int id, int textureid) override;
        void serialize(serializer::Adapter&) override;

    private:
        float m_cam_offset;
        glm::mat4 m_light_space_matrix;

        BIRDY3D_REGISTER_DERIVED_TYPE_DEC(Component, DirectionalLight);
    };

}
