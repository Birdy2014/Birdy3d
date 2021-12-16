#pragma once

#include "core/Base.hpp"
#include "core/ResourceHandle.hpp"
#include "ecs/Component.hpp"
#include "render/Forward.hpp"

namespace Birdy3d::render {

    class Camera : public ecs::Component {
    public:
        bool display_normals = false;

        Camera();
        Camera(int width, int height, bool deferred);
        void start() override;
        void cleanup() override;
        void render();
        void render_outline(const ecs::Entity*);
        void render_collider_wireframe();
        void resize(int width, int height);
        void serialize(serializer::Adapter&) override;

    private:
        unsigned int m_gbuffer, m_gbuffer_position, m_gbuffer_normal, m_gbuffer_albedo_spec, m_rbo_depth;
        int m_width, m_height;
        glm::mat4 m_projection;
        unsigned int m_quad_vao = 0;
        unsigned int m_quad_vbo;
        core::ResourceHandle<Shader> m_deferred_geometry_shader, m_deferred_light_shader, m_forward_shader, m_normal_shader, m_simple_color_shader;
        bool m_deferred_enabled;
        unsigned int m_outline_vao = 0;
        unsigned int m_outline_vbo = 0;
        unsigned int m_ssao_fbo, m_ssao_buffer;
        unsigned int m_ssao_blur_fbo, m_ssao_blur_buffer;
        unsigned int m_ssao_noise;
        core::ResourceHandle<Shader> m_ssao_shader, m_ssao_blur_shader;

        std::vector<std::shared_ptr<ModelComponent>> m_models;

        void create_gbuffer();
        void delete_gbuffer();
        void render_quad();
        void render_deferred();
        void render_forward(bool renderOpaque);
        void render_normals();

        BIRDY3D_REGISTER_DERIVED_TYPE_DEC(Component, Camera);
    };

}
