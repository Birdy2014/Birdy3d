#pragma once

#include "ecs/Component.hpp"
#include "render/Shader.hpp"

namespace Birdy3d::render {

    class DirectionalLight : public ecs::Component {
    public:
        glm::vec3 ambient;
        glm::vec3 diffuse;
        bool shadow_enabled;

        DirectionalLight(glm::vec3 ambient = glm::vec3(0), glm::vec3 diffuse = glm::vec3(0), bool shadow_enabled = true);
        void setup_shadow_map();
        void gen_shadow_map();
        void use(const Shader& lightShader, int id, int textureid);
        void start() override;
        void update() override;
        void serialize(serializer::Adapter&) override;

    private:
        float m_cam_offset;
        glm::mat4 m_light_space_transform;
        std::shared_ptr<Shader> m_depth_shader;
        unsigned int m_shadow_map_fbo, m_shadow_map;
        const unsigned int SHADOW_WIDTH = 2048, SHADOW_HEIGHT = 2048;
        bool m_shadow_map_updated = false;

        BIRDY3D_REGISTER_DERIVED_TYPE_DEC(ecs::Component, DirectionalLight);
    };

}
