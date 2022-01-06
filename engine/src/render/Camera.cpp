#include "render/Camera.hpp"

#include "core/Application.hpp"
#include "core/Logger.hpp"
#include "core/ResourceManager.hpp"
#include "ecs/Entity.hpp"
#include "ecs/Scene.hpp"
#include "physics/Collider.hpp"
#include "render/DirectionalLight.hpp"
#include "render/ModelComponent.hpp"
#include "render/PointLight.hpp"
#include "render/Shader.hpp"
#include "render/Spotlight.hpp"
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <map>
#include <random>

namespace Birdy3d::render {

    Camera::Camera()
        : m_width(1)
        , m_height(1)
        , m_deferred_enabled(false)
        , m_gbuffer(m_width, m_height)
        , m_ssao_target(m_width, m_height)
        , m_ssao_blur_target(m_width, m_height) { }

    Camera::Camera(int width, int height, bool deferred)
        : m_width(width)
        , m_height(height)
        , m_deferred_enabled(deferred)
        , m_gbuffer(m_width, m_height)
        , m_ssao_target(m_width, m_height)
        , m_ssao_blur_target(m_width, m_height) { }

    void Camera::start() {
        m_projection = glm::perspective(glm::radians(80.0f), (float)m_width / (float)m_height, 0.1f, 100.0f);

        m_gbuffer_position = m_gbuffer.add_texture(Texture::Preset::COLOR_RGBA_FLOAT);
        m_gbuffer_normal = m_gbuffer.add_texture(Texture::Preset::COLOR_RGBA_FLOAT);
        m_gbuffer_albedo_spec = m_gbuffer.add_texture(Texture::Preset::COLOR_RGBA);
        m_gbuffer.add_depth_rbo();
        if (!m_gbuffer.finish())
            core::Logger::critical("Gbuffer not complete!");

        m_ssao_texture = m_ssao_target.add_texture(Texture::Preset::COLOR_R_FLOAT);
        if (!m_ssao_target.finish())
            core::Logger::critical("SSAO FBO not complete!");

        m_ssao_blur_texture = m_ssao_blur_target.add_texture(Texture::Preset::COLOR_R_FLOAT);
        if (!m_ssao_blur_target.finish())
            core::Logger::critical("SSAO blur FBO not complete!");

        m_deferred_geometry_shader = core::ResourceManager::get_shader("geometry_buffer.glsl");
        m_deferred_light_shader = core::ResourceManager::get_shader("deferred_lighting.glsl");
        m_forward_shader = core::ResourceManager::get_shader("forward_lighting.glsl");
        m_normal_shader = core::ResourceManager::get_shader("normal_display.glsl");
        m_simple_color_shader = core::ResourceManager::get_shader("simple_color.glsl");
        m_ssao_shader = core::ResourceManager::get_shader("ssao.glsl");
        m_ssao_blur_shader = core::ResourceManager::get_shader("ssao_blur.glsl");
        m_deferred_light_shader->use();
        m_deferred_light_shader->set_int("gPosition", 0);
        m_deferred_light_shader->set_int("gNormal", 1);
        m_deferred_light_shader->set_int("gAlbedoSpec", 2);
        m_deferred_light_shader->set_int("ssao", 3);
        m_ssao_shader->use();
        m_ssao_shader->set_int("gPosition", 0);
        m_ssao_shader->set_int("gNormal", 1);
        m_ssao_shader->set_int("texNoise", 2);
        m_ssao_blur_shader->use();
        m_ssao_blur_shader->set_int("ssaoInput", 0);

        // SSAO noise texture
        std::uniform_real_distribution<GLfloat> random_floats(0.0, 1.0);
        std::default_random_engine generator;
        std::array<glm::vec3, 16> ssao_noise;
        for (unsigned int i = 0; i < ssao_noise.size(); i++) {
            glm::vec3 noise(random_floats(generator) * 2.0 - 1.0, random_floats(generator) * 2.0 - 1.0, 0.0f); // rotate around z-axis (in tangent space)
            ssao_noise[i] = glm::normalize(noise);
        }
        glGenTextures(1, &m_ssao_noise);
        glBindTexture(GL_TEXTURE_2D, m_ssao_noise);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, 4, 4, 0, GL_RGB, GL_FLOAT, &ssao_noise[0]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    }

    void Camera::cleanup() {
        if (m_outline_vao != 0) {
            glDeleteVertexArrays(1, &m_outline_vao);
            glDeleteBuffers(1, &m_outline_vbo);
            m_outline_vao = 0;
            m_outline_vbo = 0;
        }
    }

    void Camera::resize(int width, int height) {
        if (m_width != width || m_height != height) {
            m_width = width;
            m_height = height;
            m_projection = glm::perspective(glm::radians(80.0f), (float)width / (float)height, 0.1f, 100.0f);
            m_gbuffer.resize(width, height);
            m_ssao_target.resize(width, height);
            m_ssao_blur_target.resize(width, height);
        }
    }

    void Camera::render() {
        entity->scene->m_current_camera = this;
        glClearColor(0.0, 0.0, 0.0, 1.0);

        m_models.clear();
        entity->scene->get_components<ModelComponent>(m_models, false, true);

        m_dirlights.clear();
        m_pointlights.clear();
        m_spotlights.clear();
        entity->scene->get_components<DirectionalLight>(m_dirlights, false, true);
        entity->scene->get_components<PointLight>(m_pointlights, false, true);
        entity->scene->get_components<Spotlight>(m_spotlights, false, true);
        if (m_dirlights.size() != m_dirlight_amount || m_pointlights.size() != m_pointlight_amount || m_spotlights.size() != m_spotlight_amount) {
            auto shader_options_string = "DIRECTIONAL_LIGHTS_AMOUNT=" + std::to_string(m_dirlights.size()) + ":POINTLIGHTS_AMOUNT=" + std::to_string(m_pointlights.size()) + ":SPOTLIGHTS_AMOUNT=" + std::to_string(m_spotlights.size());
            m_deferred_light_shader = "file::deferred_lighting.glsl:" + shader_options_string;
            m_forward_shader = "file::forward_lighting.glsl:" + shader_options_string;
            m_dirlight_amount = m_dirlights.size();
            m_pointlight_amount = m_pointlights.size();
            m_spotlight_amount = m_spotlights.size();
        }

        if (m_deferred_enabled) {
            render_deferred();
            render_forward(false);
        } else {
            render_forward(true);
        }

        if (display_normals)
            render_normals();
    }

    void Camera::render_quad() {
        if (m_quad_vao == 0) {
            float quad_vertices[] = {
                // clang-format off
                // positions        // texture Coords
                -1.0f, 1.0f,  0.0f, 0.0f, 1.0f,
                -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
                1.0f,  1.0f,  0.0f, 1.0f, 1.0f,
                1.0f,  -1.0f, 0.0f, 1.0f, 0.0f,
                // clang-format on
            };
            // setup plane VAO
            glGenVertexArrays(1, &m_quad_vao);
            glGenBuffers(1, &m_quad_vbo);
            glBindVertexArray(m_quad_vao);
            glBindBuffer(GL_ARRAY_BUFFER, m_quad_vbo);
            glBufferData(GL_ARRAY_BUFFER, sizeof(quad_vertices), &quad_vertices, GL_STATIC_DRAW);
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
            glEnableVertexAttribArray(1);
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
        }
        glBindVertexArray(m_quad_vao);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glBindVertexArray(0);
    }

    void Camera::render_deferred() {
        glm::vec3 world_pos = entity->transform.world_position();
        glm::vec3 aworld_forward = entity->world_forward();
        glm::vec3 up = entity->world_up();
        glm::mat4 view = glm::lookAt(world_pos, world_pos + aworld_forward, up);

        auto lerp = [](float a, float b, float f) {
            return a + f * (b - a);
        };

        // Create SSAO sample kernel and noise
        std::uniform_real_distribution<float> random_floats(0.0, 1.0);
        std::default_random_engine generator;
        std::array<glm::vec3, 16> ssao_kernel;
        for (unsigned int i = 0; i < ssao_kernel.size(); ++i) {
            glm::vec3 sample(
                random_floats(generator) * 2.0 - 1.0,
                random_floats(generator) * 2.0 - 1.0,
                random_floats(generator));
            sample = glm::normalize(sample);
            sample *= random_floats(generator);
            float scale = float(i) / ssao_kernel.size();
            scale = lerp(0.1f, 1.0f, scale * scale);
            sample *= scale;
            ssao_kernel[i] = sample;
        }

        // 1. geometry pass: render all geometric/color data to g-buffer
        glEnable(GL_DEPTH_TEST);
        glDisable(GL_BLEND);
        m_gbuffer.bind();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        m_deferred_geometry_shader->use();
        m_deferred_geometry_shader->set_mat4("projection", m_projection);
        m_deferred_geometry_shader->set_mat4("view", view);
        for (auto m : m_models) {
            m->render(*m_deferred_geometry_shader, false);
        }

        // 2. SSAO
        m_ssao_target.bind();
        glClear(GL_COLOR_BUFFER_BIT);
        m_gbuffer_position->bind(0);
        m_gbuffer_normal->bind(1);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, m_ssao_noise);
        m_ssao_shader->use();
        for (unsigned int i = 0; i < ssao_kernel.size(); i++)
            m_ssao_shader->set_vec3("samples[" + std::to_string(i) + "]", ssao_kernel[i]);
        m_ssao_shader->set_mat4("projection", m_projection);
        m_ssao_shader->set_mat4("view", view);
        render_quad();

        // 3. blur SSAO
        m_ssao_blur_target.bind();
        glClear(GL_COLOR_BUFFER_BIT);
        m_ssao_texture->bind(0);
        m_ssao_blur_shader->use();
        render_quad();

        // 4. lighting pass
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClear(GL_COLOR_BUFFER_BIT);
        m_gbuffer_position->bind(0);
        m_gbuffer_normal->bind(1);
        m_gbuffer_albedo_spec->bind(2);
        m_ssao_blur_texture->bind(3);
        m_deferred_light_shader->use();
        for (size_t i = 0; i < m_dirlights.size(); i++)
            m_dirlights[i]->use(*m_deferred_light_shader, i, 4 + i);
        for (size_t i = 0; i < m_pointlights.size(); i++)
            m_pointlights[i]->use(*m_deferred_light_shader, i, 4 + m_dirlights.size() + i);
        for (size_t i = 0; i < m_spotlights.size(); i++)
            m_spotlights[i]->use(*m_deferred_light_shader, i, 4 + m_dirlights.size() + m_pointlights.size() + i);

        m_deferred_light_shader->set_vec3("viewPos", world_pos);
        render_quad();
    }

    void Camera::render_forward(bool renderOpaque) {
        glm::vec3 world_pos = entity->transform.world_position();
        glm::vec3 world_forward = entity->world_forward();
        glm::vec3 up = entity->world_up();
        glm::mat4 view = glm::lookAt(world_pos, world_pos + world_forward, up);

        glEnable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        m_forward_shader->use();
        for (size_t i = 0; i < m_dirlights.size(); i++)
            m_dirlights[i]->use(*m_forward_shader, i, 4 + i);
        for (size_t i = 0; i < m_pointlights.size(); i++)
            m_pointlights[i]->use(*m_forward_shader, i, 4 + m_dirlights.size() + i);
        for (size_t i = 0; i < m_spotlights.size(); i++)
            m_spotlights[i]->use(*m_forward_shader, i, 4 + m_dirlights.size() + m_pointlights.size() + i);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        if (renderOpaque) {
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        } else {
            glBindFramebuffer(GL_READ_FRAMEBUFFER, m_gbuffer.id());
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
            glBlitFramebuffer(0, 0, m_width, m_height, 0, 0, m_width, m_height, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
        }

        m_forward_shader->set_mat4("projection", m_projection);
        m_forward_shader->set_mat4("view", view);
        m_forward_shader->set_vec3("viewPos", world_pos);
        if (renderOpaque) {
            for (auto m : m_models) {
                m->render(*m_forward_shader, false);
            }
        }

        // Transparency
        std::map<float, ModelComponent*> sorted;
        for (auto m : m_models) {
            float distance = glm::length(entity->transform.position - m->entity->transform.position);
            sorted[distance] = m.get();
        }

        for (auto it = sorted.rbegin(); it != sorted.rend(); it++) {
            it->second->render(*m_forward_shader, true);
        }
    }

    void Camera::render_normals() {
        glm::vec3 world_pos = entity->transform.world_position();
        glm::vec3 world_forward = entity->world_forward();
        glm::vec3 up = entity->world_up();
        glm::mat4 view = glm::lookAt(world_pos, world_pos + world_forward, up);

        glEnable(GL_DEPTH_TEST);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        m_normal_shader->use();
        m_normal_shader->set_mat4("projection", m_projection);
        m_normal_shader->set_mat4("view", view);
        for (auto m : m_models) {
            m->render(*m_normal_shader, false);
            m->render(*m_normal_shader, true);
        }
    }

    void Camera::render_outline(const ecs::Entity* selected_entity) {
        if (selected_entity == nullptr)
            return;

        if (m_outline_vao == 0) {
            glGenVertexArrays(1, &m_outline_vao);
            glGenBuffers(1, &m_outline_vbo);

            glBindVertexArray(m_outline_vao);
            glBindBuffer(GL_ARRAY_BUFFER, m_outline_vbo);

            glm::vec3 vertices[24];
            glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices[0], GL_STATIC_DRAW);

            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
        }

        const float outline_offset = 0.4;

        glm::vec3 low(std::numeric_limits<float>::infinity());
        glm::vec3 high(-std::numeric_limits<float>::infinity());

        std::pair<glm::vec3, glm::vec3> bounding_box;
        glm::mat4 model;
        for (auto model_component : selected_entity->get_components<ModelComponent>(false, true)) {
            model = model_component->entity->transform.matrix();
            bounding_box = model_component->model()->bounding_box();
            glm::vec3 model_low = model * glm::vec4(bounding_box.first, 1.0f);
            glm::vec3 model_high = model * glm::vec4(bounding_box.second, 1.0f);
            if (model_low.x < low.x)
                low.x = model_low.x;
            if (model_low.y < low.y)
                low.y = model_low.y;
            if (model_low.z < low.z)
                low.z = model_low.z;
            if (model_high.x < low.x)
                low.x = model_high.x;
            if (model_high.y < low.y)
                low.y = model_high.y;
            if (model_high.z < low.z)
                low.z = model_high.z;
            if (model_high.x > high.x)
                high.x = model_high.x;
            if (model_high.y > high.y)
                high.y = model_high.y;
            if (model_high.z > high.z)
                high.z = model_high.z;
            if (model_low.x > high.x)
                high.x = model_low.x;
            if (model_low.y > high.y)
                high.y = model_low.y;
            if (model_low.z > high.z)
                high.z = model_low.z;
        }

        low -= outline_offset;
        high += outline_offset;

        // clang-format off
        glm::vec3 vertices[24] = {
            // Bottom rectangle
            glm::vec3(low.x, low.y, low.z), glm::vec3(high.x, low.y, low.z),
            glm::vec3(high.x, low.y, low.z), glm::vec3(high.x, low.y, high.z),
            glm::vec3(high.x, low.y, high.z), glm::vec3(low.x, low.y, high.z),
            glm::vec3(low.x, low.y, high.z), glm::vec3(low.x, low.y, low.z),
            // Top rectangle
            glm::vec3(low.x, high.y, low.z), glm::vec3(high.x, high.y, low.z),
            glm::vec3(high.x, high.y, low.z), glm::vec3(high.x, high.y, high.z),
            glm::vec3(high.x, high.y, high.z), glm::vec3(low.x, high.y, high.z),
            glm::vec3(low.x, high.y, high.z), glm::vec3(low.x, high.y, low.z),
            // Side rectangles
            glm::vec3(low.x, low.y, low.z), glm::vec3(low.x, high.y, low.z),
            glm::vec3(high.x, low.y, low.z), glm::vec3(high.x, high.y, low.z),
            glm::vec3(high.x, low.y, high.z), glm::vec3(high.x, high.y, high.z),
            glm::vec3(low.x, low.y, high.z), glm::vec3(low.x, high.y, high.z),
        };
        // clang-format on
        glBindVertexArray(m_outline_vao);
        glBindBuffer(GL_ARRAY_BUFFER, m_outline_vbo);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), &vertices[0]);

        glm::vec3 world_pos = entity->transform.world_position();
        glm::vec3 world_forward = entity->world_forward();
        glm::vec3 up = entity->world_up();
        glm::mat4 view = glm::lookAt(world_pos, world_pos + world_forward, up);

        glEnable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClear(GL_DEPTH_BUFFER_BIT);

        m_simple_color_shader->use();
        m_simple_color_shader->set_mat4("projection", m_projection);
        m_simple_color_shader->set_mat4("view", view);
        m_simple_color_shader->set_vec4("color", utils::Color("#e0902180"));
        m_simple_color_shader->set_mat4("model", glm::mat4(1));
        glBindVertexArray(m_outline_vao);
        glDrawArrays(GL_LINES, 0, 24);
    }

    void Camera::render_collider_wireframe() {
        glm::vec3 world_pos = entity->transform.world_position();
        glm::vec3 world_forward = entity->world_forward();
        glm::vec3 up = entity->world_up();
        glm::mat4 view = glm::lookAt(world_pos, world_pos + world_forward, up);

        glEnable(GL_DEPTH_TEST);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClear(GL_DEPTH_BUFFER_BIT);

        glDisable(GL_CULL_FACE);
        m_simple_color_shader->use();
        m_simple_color_shader->set_mat4("projection", m_projection);
        m_simple_color_shader->set_mat4("view", view);
        m_simple_color_shader->set_vec4("color", utils::Color("#00ff0080"));
        for (auto c : entity->scene->get_components<physics::Collider>(false, true)) {
            c->render_wireframe(*m_simple_color_shader);
        }
        glEnable(GL_CULL_FACE);
    }

    void Camera::serialize(serializer::Adapter& adapter) {
        adapter("deferred", m_deferred_enabled);
    }

    BIRDY3D_REGISTER_DERIVED_TYPE_DEF(ecs::Component, Camera);

}
