#include "render/DirectionalLight.hpp"

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
        , shadow_enabled(shadow_enabled)
        , m_shadow_rendertarget(SHADOW_WIDTH, SHADOW_HEIGHT) {
        m_cam_offset = 1000;
    }

    void DirectionalLight::setup_shadow_map() {
        m_depth_shader = core::ResourceManager::get_shader("directional_light_depth.glsl");
        m_shadow_map = m_shadow_rendertarget.add_texture(Texture::Preset::DEPTH);
        m_shadow_rendertarget.finish();

        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void DirectionalLight::use(const Shader& light_shader, int id, int textureid) {
        if (!m_shadow_map_updated) {
            gen_shadow_map();
            m_shadow_map_updated = true;
        }
        std::string name = "dirLights[" + std::to_string(id) + "].";
        light_shader.use();
        light_shader.set_bool(name + "shadow_enabled", shadow_enabled);
        light_shader.set_vec3(name + "position", entity->scene->m_current_camera->entity->transform.world_position() - entity->world_forward() * m_cam_offset);
        light_shader.set_vec3(name + "direction", entity->world_forward());
        light_shader.set_vec3(name + "ambient", color.value * intensity_ambient);
        light_shader.set_vec3(name + "diffuse", color.value * intensity_diffuse);
        m_shadow_map->bind(textureid);
        light_shader.set_mat4(name + "lightSpaceMatrix", m_light_space_transform);
        light_shader.set_int(name + "shadowMap", textureid);
        // TODO: cascaded shadow map
    }

    void DirectionalLight::gen_shadow_map() {
        glm::vec3 world_pos = entity->scene->m_current_camera->entity->transform.world_position() - entity->world_forward() * m_cam_offset;

        m_shadow_rendertarget.bind();
        glClear(GL_DEPTH_BUFFER_BIT);
        glCullFace(GL_FRONT);
        glEnable(GL_DEPTH_TEST);

        m_depth_shader->use();
        float near_plane = 1.0f, far_plane = 10000.0f;
        glm::mat4 light_projection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
        glm::mat4 light_view = glm::lookAt(world_pos, world_pos + entity->world_forward(), entity->world_up());
        m_light_space_transform = light_projection * light_view;
        m_depth_shader->set_mat4("lightSpaceMatrix", m_light_space_transform);
        for (auto m : entity->scene->get_components<ModelComponent>(false, true)) {
            m->render_depth(*m_depth_shader);
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

    BIRDY3D_REGISTER_DERIVED_TYPE_DEF(ecs::Component, DirectionalLight);

}
