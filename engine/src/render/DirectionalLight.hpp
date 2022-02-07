#pragma once

#include "core/ResourceHandle.hpp"
#include "ecs/Component.hpp"
#include "render/Rendertarget.hpp"
#include "render/Shader.hpp"

namespace Birdy3d::render {

    class DirectionalLight : public ecs::Component {
    public:
        utils::Color color;
        float intensity_ambient;
        float intensity_diffuse;
        bool shadow_enabled;

        DirectionalLight(utils::Color color = utils::Color::WHITE, float intensity_ambient = 1, float intensity_diffuse = 1, bool shadow_enabled = true);
        void setup_shadow_map();
        void gen_shadow_map();
        void use(const Shader& light_shader, int id, int textureid);
        void start() override;
        void update() override;
        void serialize(serializer::Adapter&) override;

    private:
        const unsigned int SHADOW_SIZE = 2048;
        float m_cam_offset = 1000.0f;
        std::vector<float> m_shadow_cascade_levels;
        std::vector<glm::mat4> m_light_space_transforms;
        core::ResourceHandle<Shader> m_depth_shader;
        GLuint m_shadow_map_fbo, m_shadow_map;
        bool m_shadow_map_updated = false;

        glm::mat4 calculate_light_space_matrix(const float near_plane, const float far_plane);

        BIRDY3D_REGISTER_DERIVED_TYPE_DEC(ecs::Component, DirectionalLight);
    };

}
