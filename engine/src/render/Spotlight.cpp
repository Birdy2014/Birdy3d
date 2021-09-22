#include "render/Spotlight.hpp"

#include "core/RessourceManager.hpp"
#include "render/ModelComponent.hpp"
#include "render/Shader.hpp"
#include "scene/Entity.hpp"
#include "scene/Scene.hpp"
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>

namespace Birdy3d {

    Spotlight::Spotlight(glm::vec3 ambient, glm::vec3 diffuse, float innerCutOff, float outerCutOff, float linear, float quadratic, bool shadow_enabled)
        : Light(shadow_enabled)
        , ambient(ambient)
        , diffuse(diffuse)
        , linear(linear)
        , quadratic(quadratic)
        , m_innerCutOff(innerCutOff)
        , m_outerCutOff(outerCutOff) { }

    void Spotlight::setup_shadow_map() {
        m_depthShader = RessourceManager::get_shader("directional_light_depth");
        // framebuffer
        glGenFramebuffers(1, &m_depthMapFBO);
        // shadow map
        glGenTextures(1, &m_depthMap);
        glBindTexture(GL_TEXTURE_2D, m_depthMap);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        // bind framebuffer
        glBindFramebuffer(GL_FRAMEBUFFER, m_depthMapFBO);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_depthMap, 0);
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void Spotlight::gen_shadow_map() {
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
        m_far = 25.0f;
        glm::mat4 light_projection = glm::perspective(m_outerCutOff * 2, aspect, near, m_far);
        glm::mat4 light_view = glm::lookAt(world_pos, world_pos + entity->world_forward(), entity->world_up());

        m_lightSpaceMatrix = light_projection * light_view;
        m_depthShader->set_mat4("lightSpaceMatrix", m_lightSpaceMatrix);
        for (auto m : entity->scene->get_components<ModelComponent>(false, true)) {
            m->render_depth(*m_depthShader);
        }

        // reset framebuffer and viewport
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, viewport[2], viewport[3]);
        //glClear(GL_DEPTH_BUFFER_BIT);
        glCullFace(GL_BACK);
    }

    void Spotlight::use(const Shader& lightShader, int id, int textureid) {
        if (!m_shadowMapUpdated) {
            gen_shadow_map();
            m_shadowMapUpdated = true;
        }
        std::string name = "spotlights[" + std::to_string(id) + "].";
        lightShader.use();
        lightShader.set_bool(name + "shadow_enabled", shadow_enabled);
        lightShader.set_vec3(name + "position", entity->transform.world_position());
        lightShader.set_vec3(name + "direction", entity->world_forward());
        lightShader.set_vec3(name + "ambient", ambient);
        lightShader.set_vec3(name + "diffuse", diffuse);
        lightShader.set_float(name + "innerCutOff", glm::cos(m_innerCutOff));
        lightShader.set_float(name + "outerCutOff", glm::cos(m_outerCutOff));
        lightShader.set_float(name + "linear", linear);
        lightShader.set_float(name + "quadratic", quadratic);
        glActiveTexture(GL_TEXTURE0 + textureid);
        glBindTexture(GL_TEXTURE_2D, m_depthMap);
        lightShader.set_mat4(name + "lightSpaceMatrix", m_lightSpaceMatrix);
        lightShader.set_int(name + "shadowMap", textureid);
    }

}
