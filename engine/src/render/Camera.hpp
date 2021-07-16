#pragma once

#include "core/Component.hpp"
#include <glm/glm.hpp>
#include <memory>

namespace Birdy3d {

    class Shader;

    class Camera : public Component {
    public:
        bool display_normals = false;

        Camera(int width, int height, bool deferred);
        void start() override;
        void cleanup() override;
        void render();
        void renderOutline(GameObject*);
        void resize(int width, int height);
        void reloadModels();

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

        void createGBuffer();
        void deleteGBuffer();
        void renderQuad();
        void renderDeferred();
        void renderForward(bool renderOpaque);
        void renderNormals();
    };

}
