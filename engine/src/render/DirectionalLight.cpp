#include "render/DirectionalLight.hpp"

#include "core/ResourceManager.hpp"
#include "ecs/Entity.hpp"
#include "ecs/Scene.hpp"
#include "render/Camera.hpp"
#include "render/ModelComponent.hpp"
#include "render/Shader.hpp"
#include <glm/gtc/matrix_transform.hpp>

namespace Birdy3d {

    DirectionalLight::DirectionalLight(glm::vec3 ambient, glm::vec3 diffuse, bool shadow_enabled)
        : Light(shadow_enabled)
        , ambient(ambient)
        , diffuse(diffuse) {
        m_cam_offset = 1000;
    }

    void DirectionalLight::setup_shadow_map() {
        m_depthShader = ResourceManager::get_shader("directional_light_depth");
        // framebuffer
        glGenFramebuffers(1, &m_depthMapFBO);
        // shadow map
        glGenTextures(1, &m_depthMap);
        glBindTexture(GL_TEXTURE_2D, m_depthMap);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        float border_color[] = { 1.0f, 1.0f, 1.0f, 1.0f };
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, border_color);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        // bind framebuffer
        glBindFramebuffer(GL_FRAMEBUFFER, m_depthMapFBO);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_depthMap, 0);
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void DirectionalLight::use(const Shader& lightShader, int id, int textureid) {
        if (!m_shadowMapUpdated) {
            gen_shadow_map();
            m_shadowMapUpdated = true;
        }
        std::string name = "dirLights[" + std::to_string(id) + "].";
        lightShader.use();
        lightShader.set_bool(name + "shadow_enabled", shadow_enabled);
        lightShader.set_vec3(name + "position", entity->scene->m_current_camera->entity->transform.world_position() - entity->world_forward() * m_cam_offset);
        lightShader.set_vec3(name + "direction", entity->world_forward());
        lightShader.set_vec3(name + "ambient", ambient);
        lightShader.set_vec3(name + "diffuse", diffuse);
        glActiveTexture(GL_TEXTURE0 + textureid);
        glBindTexture(GL_TEXTURE_2D, m_depthMap);
        lightShader.set_mat4(name + "lightSpaceMatrix", m_light_space_matrix);
        lightShader.set_int(name + "shadowMap", textureid);
        // TODO: cascaded shadow map
    }

    void DirectionalLight::gen_shadow_map() {
        glm::vec3 world_pos = entity->scene->m_current_camera->entity->transform.world_position() - entity->world_forward() * m_cam_offset;

        GLint viewport[4];
        glGetIntegerv(GL_VIEWPORT, viewport);
        glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
        glBindFramebuffer(GL_FRAMEBUFFER, m_depthMapFBO);
        glClear(GL_DEPTH_BUFFER_BIT);
        glCullFace(GL_FRONT);
        glEnable(GL_DEPTH_TEST);

        m_depthShader->use();
        float near_plane = 1.0f, far_plane = 10000.0f;
        glm::mat4 light_projection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
        glm::mat4 light_view = glm::lookAt(world_pos, world_pos + entity->world_forward(), entity->world_up());
        m_light_space_matrix = light_projection * light_view;
        m_depthShader->set_mat4("lightSpaceMatrix", m_light_space_matrix);
        for (auto m : entity->scene->get_components<ModelComponent>(false, true)) {
            m->render_depth(*m_depthShader);
        }

        // reset framebuffer and viewport
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, viewport[2], viewport[3]);
        //glClear(GL_DEPTH_BUFFER_BIT);
        glCullFace(GL_BACK);
    }

    void DirectionalLight::serialize(serializer::Adapter& adapter) {
        adapter("shadow_enabled", shadow_enabled);
        adapter("ambient", ambient);
        adapter("diffuse", diffuse);
        adapter("cam_offset", m_cam_offset);
    }

    BIRDY3D_REGISTER_DERIVED_TYPE_DEF(Component, DirectionalLight);

}
