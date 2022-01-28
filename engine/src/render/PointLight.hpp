#pragma once

#include "core/ResourceHandle.hpp"
#include "ecs/Component.hpp"
#include "render/Shader.hpp"
#include "utils/Color.hpp"

namespace Birdy3d::render {

    class PointLight : public ecs::Component {
    public:
        utils::Color color;
        float intensity_ambient;
        float intensity_diffuse;
        float linear;
        float quadratic;
        bool shadow_enabled;

        PointLight(utils::Color color = utils::Color::WHITE, float intensity_ambient = 1, float intensity_diffuse = 1, float linear = 0, float quadratic = 0, bool shadow_enabled = true);
        void setup_shadow_map();
        void gen_shadow_map();
        void use(const Shader& light_shader, int id, int textureid);
        void start() override;
        void update() override;
        void serialize(serializer::Adapter&) override;

    private:
        float m_far = 25.0f;
        core::ResourceHandle<Shader> m_depth_shader;
        unsigned int m_shadow_map_fbo, m_shadow_map;
        const unsigned int SHADOW_WIDTH = 2048, SHADOW_HEIGHT = 2048;
        bool m_shadow_map_updated = false;

        BIRDY3D_REGISTER_DERIVED_TYPE_DEC(ecs::Component, PointLight);
    };

}
