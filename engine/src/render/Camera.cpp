#include "render/Camera.hpp"

#include "core/Application.hpp"
#include "core/Logger.hpp"
#include "core/ResourceManager.hpp"
#include "ecs/Entity.hpp"
#include "ecs/Scene.hpp"
#include "physics/ColliderComponent.hpp"
#include "render/DirectionalLight.hpp"
#include "render/ModelComponent.hpp"
#include "render/PointLight.hpp"
#include "render/Shader.hpp"
#include "render/Spotlight.hpp"
#include "ui/Theme.hpp"
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <map>
#include <random>

namespace Birdy3d::render {

    Camera::Camera()
        : deferred_enabled(false)
        , target(Rendertarget::DEFAULT)
        , m_old_target_width(1)
        , m_old_target_height(1)
        , m_gbuffer(1, 1)
        , m_ssao_target(1, 1)
        , m_ssao_blur_target(1, 1)
    { }

    Camera::Camera(std::shared_ptr<Rendertarget> target, bool deferred)
        : deferred_enabled(deferred)
        , target(target)
        , m_old_target_width(target->width())
        , m_old_target_height(target->height())
        , m_gbuffer(target->width(), target->height())
        , m_ssao_target(target->width(), target->height())
        , m_ssao_blur_target(target->width(), target->height())
    { }

    void Camera::start()
    {
        m_projection = glm::perspective(fov, (float)m_old_target_width / (float)m_old_target_height, near, far);

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
        m_ssao_shader->use();
        m_ssao_shader->set_int("gbuffer_position", 0);
        m_ssao_shader->set_int("gbuffer_normal", 1);
        m_ssao_shader->set_int("tex_noise", 2);
        m_ssao_blur_shader->use();
        m_ssao_blur_shader->set_int("ssao_input", 0);

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

    void Camera::cleanup()
    {
        if (m_outline_vao != 0) {
            glDeleteVertexArrays(1, &m_outline_vao);
            glDeleteBuffers(1, &m_outline_vbo);
            m_outline_vao = 0;
            m_outline_vbo = 0;
        }
    }

    void Camera::render()
    {
        if (!target) {
            core::Logger::error("Camera Rendertarget not set");
            return;
        }

        entity->scene->m_current_camera = this;
        glClearColor(0.0, 0.0, 0.0, 1.0);

        if (m_old_target_width != target->width() || m_old_target_height != target->height()) {
            m_old_target_width = target->width();
            m_old_target_height = target->height();
            m_projection = glm::perspective(fov, (float)target->width() / (float)target->height(), near, far);
            m_gbuffer.resize(target->width(), target->height());
            m_ssao_target.resize(target->width(), target->height());
            m_ssao_blur_target.resize(target->width(), target->height());
        }

        glm::vec3 world_pos = entity->transform.world_position();
        glm::vec3 world_forward = entity->world_forward();
        glm::vec3 up = entity->world_up();
        m_view = glm::lookAt(world_pos, world_pos + world_forward, up);

        m_models.clear();
        entity->scene->get_components<ModelComponent>(m_models, false, true);

        m_dirlights.clear();
        m_pointlights.clear();
        m_spotlights.clear();
        entity->scene->get_components<DirectionalLight>(m_dirlights, false, true);
        entity->scene->get_components<PointLight>(m_pointlights, false, true);
        entity->scene->get_components<Spotlight>(m_spotlights, false, true);
        if (m_dirlights.size() != m_dirlight_amount || m_pointlights.size() != m_pointlight_amount || m_spotlights.size() != m_spotlight_amount) {
            m_deferred_light_shader.arg("DIRECTIONAL_LIGHTS_AMOUNT", m_dirlights.size());
            m_deferred_light_shader.arg("POINTLIGHTS_AMOUNT", m_pointlights.size());
            m_deferred_light_shader.arg("SPOTLIGHTS_AMOUNT", m_spotlights.size());
            m_forward_shader.arg("DIRECTIONAL_LIGHTS_AMOUNT", m_dirlights.size());
            m_forward_shader.arg("POINTLIGHTS_AMOUNT", m_pointlights.size());
            m_forward_shader.arg("SPOTLIGHTS_AMOUNT", m_spotlights.size());
            m_dirlight_amount = m_dirlights.size();
            m_pointlight_amount = m_pointlights.size();
            m_spotlight_amount = m_spotlights.size();
        }

        if (deferred_enabled) {
            render_deferred();
            render_forward(false);
        } else {
            render_forward(true);
        }

        if (display_normals)
            render_normals();
    }

    void Camera::render_quad()
    {
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

    void Camera::render_deferred()
    {
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
        m_deferred_geometry_shader->set_mat4("view", m_view);
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
        m_ssao_shader->set_mat4("view", m_view);
        render_quad();

        // 3. blur SSAO
        m_ssao_blur_target.bind();
        glClear(GL_COLOR_BUFFER_BIT);
        m_ssao_texture->bind(0);
        m_ssao_blur_shader->use();
        render_quad();

        // 4. lighting pass
        m_gbuffer_position->bind(0);
        m_gbuffer_normal->bind(1);
        m_gbuffer_albedo_spec->bind(2);
        m_ssao_blur_texture->bind(3);
        m_deferred_light_shader.arg("SHADOW_CASCADE_SIZE", core::Application::option_int(core::IntOption::SHADOW_CASCADE_SIZE));
        for (size_t i = 0; i < m_dirlights.size(); i++)
            m_dirlights[i]->use(*m_deferred_light_shader, i, 4 + i);
        for (size_t i = 0; i < m_pointlights.size(); i++)
            m_pointlights[i]->use(*m_deferred_light_shader, i, 4 + m_dirlights.size() + i);
        for (size_t i = 0; i < m_spotlights.size(); i++)
            m_spotlights[i]->use(*m_deferred_light_shader, i, 4 + m_dirlights.size() + m_pointlights.size() + i);

        target->bind();
        glClear(GL_COLOR_BUFFER_BIT);
        glEnable(GL_FRAMEBUFFER_SRGB);
        m_deferred_light_shader->set_int("gbuffer_position", 0);
        m_deferred_light_shader->set_int("gbuffer_normal", 1);
        m_deferred_light_shader->set_int("gbuffer_albedo_spec", 2);
        m_deferred_light_shader->set_int("ssao", 3);
        m_deferred_light_shader->set_mat4("view", m_view);
        m_deferred_light_shader->set_vec3("view_pos", entity->transform.world_position());
        render_quad();
        glDisable(GL_FRAMEBUFFER_SRGB);
    }

    void Camera::render_forward(bool render_opaque)
    {
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        m_forward_shader.arg("SHADOW_CASCADE_SIZE", core::Application::option_int(core::IntOption::SHADOW_CASCADE_SIZE));
        for (size_t i = 0; i < m_dirlights.size(); i++)
            m_dirlights[i]->use(*m_forward_shader, i, 4 + i);
        for (size_t i = 0; i < m_pointlights.size(); i++)
            m_pointlights[i]->use(*m_forward_shader, i, 4 + m_dirlights.size() + i);
        for (size_t i = 0; i < m_spotlights.size(); i++)
            m_spotlights[i]->use(*m_forward_shader, i, 4 + m_dirlights.size() + m_pointlights.size() + i);

        target->bind();
        if (render_opaque) {
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        } else {
            glBindFramebuffer(GL_READ_FRAMEBUFFER, m_gbuffer.id());
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, target->id());
            glBlitFramebuffer(0, 0, target->width() - 1, target->height() - 1, 0, 0, target->width() - 1, target->height() - 1, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
        }

        glEnable(GL_FRAMEBUFFER_SRGB);
        m_forward_shader->use();
        m_forward_shader->set_mat4("projection", m_projection);
        m_forward_shader->set_mat4("view", m_view);
        m_forward_shader->set_vec3("view_pos", entity->transform.world_position());
        if (render_opaque) {
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
        glDisable(GL_FRAMEBUFFER_SRGB);
    }

    void Camera::render_normals()
    {
        glEnable(GL_DEPTH_TEST);

        target->bind();

        m_normal_shader->use();
        m_normal_shader->set_mat4("projection", m_projection);
        m_normal_shader->set_mat4("view", m_view);
        for (auto m : m_models) {
            m->render(*m_normal_shader, false);
            m->render(*m_normal_shader, true);
        }
    }

    void Camera::render_outline(ecs::Entity* selected_entity)
    {
        if (selected_entity == nullptr)
            return;

        if (m_outline_vao == 0) {
            glGenVertexArrays(1, &m_outline_vao);
            glGenBuffers(1, &m_outline_vbo);

            glBindVertexArray(m_outline_vao);
            glBindBuffer(GL_ARRAY_BUFFER, m_outline_vbo);

            glm::vec3 vertices[24];
            glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices[0], GL_DYNAMIC_DRAW);

            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
        }

        glm::vec3 low(std::numeric_limits<float>::infinity());
        glm::vec3 high(-std::numeric_limits<float>::infinity());

        std::function<void(ecs::Entity*, glm::mat4)> compute_matrix = [&low, &high, &compute_matrix](ecs::Entity* entity, glm::mat4 model) {
            for (auto model_component : entity->get_components<ModelComponent>(false, false)) {
                auto bounding_box = model_component->model()->bounding_box();
                glm::vec3 model_low = model * glm::vec4(bounding_box.first, 1.0f);
                glm::vec3 model_high = model * glm::vec4(bounding_box.second, 1.0f);
                if (model_low.x < low.x)
                    low.x = model_low.x;
                if (model_low.y < low.y)
                    low.y = model_low.y;
                if (model_low.z < low.z)
                    low.z = model_low.z;
                if (model_high.x > high.x)
                    high.x = model_high.x;
                if (model_high.y > high.y)
                    high.y = model_high.y;
                if (model_high.z > high.z)
                    high.z = model_high.z;
            }
            for (auto child : entity->children()) {
                compute_matrix(child.get(), model * child->transform.local_matrix());
            }
        };

        compute_matrix(selected_entity, glm::mat4{1});

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

        glEnable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        target->bind();
        glClear(GL_DEPTH_BUFFER_BIT);

        m_simple_color_shader->use();
        m_simple_color_shader->set_mat4("projection", m_projection);
        m_simple_color_shader->set_mat4("view", m_view);
        m_simple_color_shader->set_vec4("color", core::Application::theme().color(utils::Color::Name::OBJECT_SELECTION));
        m_simple_color_shader->set_mat4("model", selected_entity->transform.global_matrix());
        glBindVertexArray(m_outline_vao);
        glDrawArrays(GL_LINES, 0, 24);
    }

    void Camera::render_collider_wireframe()
    {
        glEnable(GL_DEPTH_TEST);
        target->bind();
        glClear(GL_DEPTH_BUFFER_BIT);

        glDisable(GL_CULL_FACE);
        m_simple_color_shader->use();
        m_simple_color_shader->set_mat4("projection", m_projection);
        m_simple_color_shader->set_mat4("view", m_view);
        m_simple_color_shader->set_vec4("color", core::Application::theme().color(utils::Color::Name::COLLIDER_WIREFRAME));
        for (auto c : entity->scene->get_components<physics::ColliderComponent>(false, true)) {
            c->render_wireframe(*m_simple_color_shader);
        }
        glEnable(GL_CULL_FACE);
    }

    void Camera::serialize(serializer::Adapter& adapter)
    {
        adapter("deferred", deferred_enabled);
        adapter("fov", fov);
        adapter("near", near);
        adapter("far", far);
    }

    BIRDY3D_REGISTER_DERIVED_TYPE_DEF(ecs::Component, Camera);

}
