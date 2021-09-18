#pragma once

#include "core/Base.hpp"
#include "scene/Component.hpp"

namespace Birdy3d {

    class Shader;

    class Camera : public Component {
    public:
        bool display_normals = false;

        Camera();
        Camera(int width, int height, bool deferred);
        void start() override;
        void cleanup() override;
        void render();
        void renderOutline(const GameObject*);
        void render_collider_wireframe();
        void resize(int width, int height);

        template <class Archive>
        void serialize(Archive& ar) {
            ar(cereal::make_nvp("deferred", m_deferred_enabled));
        }

    private:
        unsigned int m_gbuffer, m_gbuffer_position, m_gbuffer_normal, m_gbuffer_albedo_spec, m_rbo_depth;
        int m_width, m_height;
        glm::mat4 m_projection;
        unsigned int m_quad_vao = 0;
        unsigned int m_quad_vbo;
        std::shared_ptr<Shader> m_deferred_geometry_shader, m_deferred_light_shader, m_forward_shader, m_normal_shader, m_simple_color_shader;
        bool m_deferred_enabled;
        unsigned int m_outline_vao = 0;
        unsigned int m_outline_vbo = 0;
        unsigned int m_ssao_fbo, m_ssao_buffer;
        unsigned int m_ssao_blur_fbo, m_ssao_blur_buffer;
        unsigned int m_ssao_noise;
        std::shared_ptr<Shader> m_ssao_shader, m_ssao_blur_shader;

        void createGBuffer();
        void deleteGBuffer();
        void renderQuad();
        void renderDeferred();
        void renderForward(bool renderOpaque);
        void renderNormals();
    };

}

CEREAL_REGISTER_TYPE(Birdy3d::Camera);
CEREAL_REGISTER_POLYMORPHIC_RELATION(Birdy3d::Component, Birdy3d::Camera);
