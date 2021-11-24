#include "render/PointLight.hpp"

#include "core/ResourceManager.hpp"
#include "ecs/Entity.hpp"
#include "ecs/Scene.hpp"
#include "render/ModelComponent.hpp"
#include "render/Shader.hpp"
#include <glm/gtc/matrix_transform.hpp>

namespace Birdy3d {

    PointLight::PointLight(glm::vec3 ambient, glm::vec3 diffuse, float linear, float quadratic, bool shadow_enabled)
        : Light(shadow_enabled)
        , ambient(ambient)
        , diffuse(diffuse)
        , linear(linear)
        , quadratic(quadratic) { }

    void PointLight::setup_shadow_map() {
        m_depthShader = ResourceManager::get_shader("point_light_depth");
        // framebuffer
        glGenFramebuffers(1, &m_depthMapFBO);
        // shadow map
        glGenTextures(1, &m_depthMap);
        glBindTexture(GL_TEXTURE_CUBE_MAP, m_depthMap);
        for (unsigned int i = 0; i < 6; i++)
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        // bind framebuffer
        glBindFramebuffer(GL_FRAMEBUFFER, m_depthMapFBO);
        glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, m_depthMap, 0);
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void PointLight::use(const Shader& lightShader, int id, int textureid) {
        if (!m_shadowMapUpdated) {
            gen_shadow_map();
            m_shadowMapUpdated = true;
        }
        std::string name = "pointLights[" + std::to_string(id) + "].";
        lightShader.use();
        lightShader.set_bool(name + "shadow_enabled", shadow_enabled);
        lightShader.set_vec3(name + "position", entity->transform.world_position());
        lightShader.set_vec3(name + "ambient", ambient);
        lightShader.set_vec3(name + "diffuse", diffuse);
        lightShader.set_float(name + "linear", linear);
        lightShader.set_float(name + "quadratic", quadratic);
        glActiveTexture(GL_TEXTURE0 + textureid);
        glBindTexture(GL_TEXTURE_CUBE_MAP, m_depthMap);
        lightShader.set_int(name + "shadowMap", textureid);
        lightShader.set_float(name + "far", m_far);
    }

    void PointLight::gen_shadow_map() {
        glm::vec3 world_pos = entity->transform.world_position();

        GLint viewport[4];
        glGetIntegerv(GL_VIEWPORT, viewport);
        glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
        glBindFramebuffer(GL_FRAMEBUFFER, m_depthMapFBO);
        glClear(GL_DEPTH_BUFFER_BIT);
        glCullFace(GL_FRONT);
        glEnable(GL_DEPTH_TEST);

        m_depthShader->use();
        float aspect = (float)SHADOW_WIDTH / (float)SHADOW_HEIGHT;
        float near = 1.0f;
        glm::mat4 shadow_proj = glm::perspective(glm::radians(90.0f), aspect, near, m_far);

        std::vector<glm::mat4> shadow_transforms;
        shadow_transforms.push_back(shadow_proj * glm::lookAt(world_pos, world_pos + glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
        shadow_transforms.push_back(shadow_proj * glm::lookAt(world_pos, world_pos + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
        shadow_transforms.push_back(shadow_proj * glm::lookAt(world_pos, world_pos + glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0)));
        shadow_transforms.push_back(shadow_proj * glm::lookAt(world_pos, world_pos + glm::vec3(0.0, -1.0, 0.0), glm::vec3(0.0, 0.0, -1.0)));
        shadow_transforms.push_back(shadow_proj * glm::lookAt(world_pos, world_pos + glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, -1.0, 0.0)));
        shadow_transforms.push_back(shadow_proj * glm::lookAt(world_pos, world_pos + glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.0, -1.0, 0.0)));
        for (unsigned int i = 0; i < 6; i++)
            m_depthShader->set_mat4("shadowMatrices[" + std::to_string(i) + "]", shadow_transforms[i]);
        m_depthShader->set_float("far_plane", m_far);
        m_depthShader->set_vec3("lightPos", world_pos);
        for (auto m : entity->scene->get_components<ModelComponent>(false, true)) {
            m->render_depth(*m_depthShader);
        }

        // reset framebuffer and viewport
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, viewport[2], viewport[3]);
        glClear(GL_DEPTH_BUFFER_BIT);
        glCullFace(GL_BACK);
    }

    void PointLight::serialize(serializer::Adapter& adapter) {
        adapter("shadow_enabled", shadow_enabled);
        adapter("ambient", ambient);
        adapter("diffuse", diffuse);
        adapter("linear", linear);
        adapter("quadratic", quadratic);
        adapter("far", m_far);
    }

    BIRDY3D_REGISTER_DERIVED_TYPE_DEF(Component, PointLight);

}
