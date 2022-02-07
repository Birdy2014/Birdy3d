#pragma once

#include "core/Base.hpp"
#include "core/ResourceHandle.hpp"
#include "ecs/Component.hpp"
#include "render/Forward.hpp"
#include "render/Rendertarget.hpp"

namespace Birdy3d::render {

    class Camera : public ecs::Component {
    public:
        bool display_normals = false;
        bool deferred_enabled = true;
        float fov = glm::radians(80.0f);
        float near = 0.1f;
        float far = 100.0f;
        std::shared_ptr<Rendertarget> target;

        Camera();
        Camera(std::shared_ptr<Rendertarget> target, bool deferred);
        void start() override;
        void cleanup() override;
        void render();
        void render_outline(ecs::Entity*);
        void render_collider_wireframe();
        void serialize(serializer::Adapter&) override;
        glm::mat4 view() { return m_view; }
        glm::mat4 projection() { return m_projection; }

    private:
        int m_old_target_width, m_old_target_height;

        Rendertarget m_gbuffer;
        Texture *m_gbuffer_position, *m_gbuffer_normal, *m_gbuffer_albedo_spec;

        glm::mat4 m_view;
        glm::mat4 m_projection;

        unsigned int m_quad_vao = 0;
        unsigned int m_quad_vbo;
        core::ResourceHandle<Shader> m_deferred_geometry_shader, m_deferred_light_shader, m_forward_shader, m_normal_shader, m_simple_color_shader;
        unsigned int m_outline_vao = 0;
        unsigned int m_outline_vbo = 0;

        std::vector<std::shared_ptr<DirectionalLight>> m_dirlights;
        std::vector<std::shared_ptr<PointLight>> m_pointlights;
        std::vector<std::shared_ptr<Spotlight>> m_spotlights;
        // For updating shader
        std::size_t m_dirlight_amount = 0;
        std::size_t m_pointlight_amount = 0;
        std::size_t m_spotlight_amount = 0;

        Rendertarget m_ssao_target;
        Texture* m_ssao_texture;

        Rendertarget m_ssao_blur_target;
        Texture* m_ssao_blur_texture;

        GLuint m_ssao_noise;
        core::ResourceHandle<Shader> m_ssao_shader, m_ssao_blur_shader;

        std::vector<std::shared_ptr<ModelComponent>> m_models;

        void render_quad();
        void render_deferred();
        void render_forward(bool renderOpaque);
        void render_normals();

        BIRDY3D_REGISTER_DERIVED_TYPE_DEC(Component, Camera);
    };

}
