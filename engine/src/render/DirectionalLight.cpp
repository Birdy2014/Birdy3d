#include "render/DirectionalLight.hpp"

#include "core/Application.hpp"
#include "core/ResourceManager.hpp"
#include "ecs/Entity.hpp"
#include "ecs/Scene.hpp"
#include "render/Camera.hpp"
#include "render/ModelComponent.hpp"
#include "render/Shader.hpp"
#include <glm/gtc/matrix_transform.hpp>

namespace Birdy3d::render {

    DirectionalLight::DirectionalLight(utils::Color color, float intensity_ambient, float intensity_diffuse, bool shadow_enabled)
        : color(color)
        , intensity_ambient(intensity_ambient)
        , intensity_diffuse(intensity_diffuse)
        , shadow_enabled(shadow_enabled) { }

    void DirectionalLight::setup_shadow_map() {
        m_depth_shader = "directional_light_depth.glsl";
        m_depth_tesselation_shader = "directional_light_depth_tesselation.glsl";

        glGenFramebuffers(1, &m_shadow_map_fbo);

        glGenTextures(1, &m_shadow_map);
        glBindTexture(GL_TEXTURE_2D_ARRAY, m_shadow_map);
        glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_DEPTH_COMPONENT32F, SHADOW_SIZE, SHADOW_SIZE, core::Application::option_int(core::IntOption::SHADOW_CASCADE_SIZE), 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);

        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);

        constexpr float bordercolor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
        glTexParameterfv(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_BORDER_COLOR, bordercolor);

        glBindFramebuffer(GL_FRAMEBUFFER, m_shadow_map_fbo);
        glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, m_shadow_map, 0);
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);

        int status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        if (status != GL_FRAMEBUFFER_COMPLETE) {
            core::Logger::error("ERROR::FRAMEBUFFER:: Framebuffer is not complete!");
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void DirectionalLight::use(const Shader& light_shader, int id, int textureid) {
        if (!m_shadow_map_updated) {
            gen_shadow_map();
            m_shadow_map_updated = true;
        }
        std::string name = "directional_lights[" + std::to_string(id) + "].";
        light_shader.use();
        light_shader.set_bool(name + "shadow_enabled", shadow_enabled);
        light_shader.set_vec3(name + "position", entity->scene->m_current_camera->entity->transform.world_position() - entity->world_forward() * m_cam_offset);
        light_shader.set_vec3(name + "direction", entity->world_forward());
        light_shader.set_vec3(name + "ambient", color.value * intensity_ambient);
        light_shader.set_vec3(name + "diffuse", color.value * intensity_diffuse);
        glActiveTexture(GL_TEXTURE0 + textureid);
        glBindTexture(GL_TEXTURE_2D_ARRAY, m_shadow_map);
        for (int i = 0; i < core::Application::option_int(core::IntOption::SHADOW_CASCADE_SIZE); ++i) {
            light_shader.set_mat4(name + "light_space_matrices[" + std::to_string(i) + "]", m_light_space_transforms[i]);
            light_shader.set_float(name + "shadow_cascade_levels[" + std::to_string(i) + "]", m_shadow_cascade_levels[i]);
        }
        light_shader.set_int(name + "shadow_map", textureid);
    }

    void DirectionalLight::gen_shadow_map() {
        glBindFramebuffer(GL_FRAMEBUFFER, m_shadow_map_fbo);
        glClear(GL_DEPTH_BUFFER_BIT);
        glCullFace(GL_FRONT);
        glEnable(GL_DEPTH_TEST);
        glViewport(0, 0, SHADOW_SIZE, SHADOW_SIZE);

        int shadow_cascade_size = core::Application::option_int(core::IntOption::SHADOW_CASCADE_SIZE);
        m_depth_shader.arg("SHADOW_CASCADE_SIZE", shadow_cascade_size);
        m_depth_tesselation_shader.arg("SHADOW_CASCADE_SIZE", shadow_cascade_size);

        float nearest = 5.0f;
        float camera_near = entity->scene->m_current_camera->near;
        float camera_far = entity->scene->m_current_camera->far;
        m_light_space_transforms.reserve(shadow_cascade_size);
        m_shadow_cascade_levels.reserve(shadow_cascade_size);
        for (int i = 0; i < shadow_cascade_size; ++i) {
            // TODO: Use exponential scale instead of linear
            float near = i == 0 ? camera_near : m_shadow_cascade_levels[i - 1];
            float far = i == 0 ? nearest : near + (camera_far - near) / (shadow_cascade_size - i);
            m_shadow_cascade_levels[i] = far;
            m_light_space_transforms[i] = calculate_light_space_matrix(near, far);
            m_depth_shader->set_mat4("light_space_matrices[" + std::to_string(i) + "]", m_light_space_transforms[i]);
            m_depth_tesselation_shader->set_mat4("light_space_matrices[" + std::to_string(i) + "]", m_light_space_transforms[i]);
        }
        for (auto m : entity->scene->get_components<ModelComponent>(false, true)) {
            if (auto material = m->material(); material ? material->height_map_enabled : false) {
                m_depth_tesselation_shader->use();
                m->render_depth(*m_depth_tesselation_shader);
            } else {
                m_depth_shader->use();
                m->render_depth(*m_depth_shader);
            }
        }

        glCullFace(GL_BACK);
    }

    void DirectionalLight::start() {
        setup_shadow_map();
    }

    void DirectionalLight::update() {
        if (shadow_enabled)
            m_shadow_map_updated = false;
    }

    void DirectionalLight::serialize(serializer::Adapter& adapter) {
        adapter("shadow_enabled", shadow_enabled);
        adapter("color", color);
        adapter("intensity_ambient", intensity_ambient);
        adapter("intensity_diffuse", intensity_diffuse);
        adapter("cam_offset", m_cam_offset);
    }

    glm::mat4 DirectionalLight::calculate_light_space_matrix(const float near_plane, const float far_plane) {
        const auto camera = entity->scene->m_current_camera;
        const auto projection = glm::perspective(camera->fov, (float)camera->target->width() / (float)camera->target->height(), near_plane, far_plane);
        const auto view = entity->scene->m_current_camera->view();

        const auto inv = glm::inverse(projection * view);

        std::vector<glm::vec4> frustum_corners;
        for (unsigned int x = 0; x < 2; ++x) {
            for (unsigned int y = 0; y < 2; ++y) {
                for (unsigned int z = 0; z < 2; ++z) {
                    const glm::vec4 pt = inv * glm::vec4(2.0f * x - 1.0f, 2.0f * y - 1.0f, 2.0f * z - 1.0f, 1.0f);
                    frustum_corners.push_back(pt / pt.w);
                }
            }
        }

        glm::vec3 center = glm::vec3(0, 0, 0);
        for (const auto& v : frustum_corners) {
            center += glm::vec3(v);
        }
        center /= frustum_corners.size();

        const auto light_view = glm::lookAt(center - entity->world_forward(), center, entity->world_up());

        float min_x = std::numeric_limits<float>::max();
        float max_x = std::numeric_limits<float>::min();
        float min_y = std::numeric_limits<float>::max();
        float max_y = std::numeric_limits<float>::min();
        float min_z = std::numeric_limits<float>::max();
        float max_z = std::numeric_limits<float>::min();
        for (const auto& v : frustum_corners) {
            const auto trf = light_view * v;
            min_x = std::min(min_x, trf.x);
            max_x = std::max(max_x, trf.x);
            min_y = std::min(min_y, trf.y);
            max_y = std::max(max_y, trf.y);
            min_z = std::min(min_z, trf.z);
            max_z = std::max(max_z, trf.z);
        }

        if (min_z < 0) {
            min_z *= m_cam_offset;
        } else {
            min_z /= m_cam_offset;
        }
        if (max_z < 0) {
            max_z /= m_cam_offset;
        } else {
            max_z *= m_cam_offset;
        }

        const glm::mat4 light_projection = glm::ortho(min_x, max_x, min_y, max_y, min_z, max_z);

        return light_projection * light_view;
    }

    BIRDY3D_REGISTER_DERIVED_TYPE_DEF(ecs::Component, DirectionalLight);

}
