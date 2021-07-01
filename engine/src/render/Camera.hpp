#pragma once

#include "core/Component.hpp"
#include <glm/glm.hpp>
#include <memory>

namespace Birdy3d {

    class Shader;
    class Canvas;

    class Camera : public Component {
    public:
        Canvas* canvas = nullptr;

        Camera(int width, int height, bool deferred);
        Camera(int width, int height, bool deferred, Canvas* canvas);
        void start() override;
        void cleanup() override;
        void render();
        void resize(int width, int height);
        void reloadModels();

    private:
        unsigned int m_gbuffer, m_gbuffer_position, m_gbuffer_normal, m_gbuffer_albedo_spec, m_rbo_depth;
        int m_width, m_height;
        glm::mat4 m_projection;
        unsigned int m_quad_vao = 0;
        unsigned int m_quad_vbo;
        std::shared_ptr<Shader> m_deferred_geometry_shader, m_deferred_light_shader, m_forward_shader;
        bool m_deferred_enabled;

        void createGBuffer();
        void deleteGBuffer();
        void renderQuad();
        void renderDeferred();
        void renderForward(bool renderOpaque);
    };

}
