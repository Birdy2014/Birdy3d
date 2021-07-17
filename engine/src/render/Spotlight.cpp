#include "render/Spotlight.hpp"

#include "scene/GameObject.hpp"
#include "core/RessourceManager.hpp"
#include "render/ModelComponent.hpp"
#include "render/Shader.hpp"
#include "scene/Scene.hpp"
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>

namespace Birdy3d {

    Spotlight::Spotlight(glm::vec3 ambient, glm::vec3 diffuse, float innerCutOff, float outerCutOff, float linear, float quadratic, bool shadow_enabled)
        : Light(RessourceManager::getShader("directional_light_depth"), ambient, diffuse, linear, quadratic, shadow_enabled)
        , m_innerCutOff(innerCutOff)
        , m_outerCutOff(outerCutOff) { }

    void Spotlight::setupShadowMap() {
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

    void Spotlight::genShadowMap() {
        glm::vec3 absPos = object->transform.worldPosition();

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
        glm::mat4 lightProjection = glm::perspective(m_outerCutOff * 2, aspect, near, m_far);
        glm::mat4 lightView = glm::lookAt(absPos, absPos + object->absForward(), object->absUp());

        m_lightSpaceMatrix = lightProjection * lightView;
        m_depthShader->setMat4("lightSpaceMatrix", m_lightSpaceMatrix);
        for (ModelComponent* m : object->scene->getComponents<ModelComponent>(false, true)) {
            m->renderDepth(m_depthShader.get());
        }

        // reset framebuffer and viewport
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, viewport[2], viewport[3]);
        //glClear(GL_DEPTH_BUFFER_BIT);
        glCullFace(GL_BACK);
    }

    void Spotlight::use(Shader* lightShader, int id, int textureid) {
        if (!m_shadowMapUpdated) {
            genShadowMap();
            m_shadowMapUpdated = true;
        }
        std::string name = "spotlights[" + std::to_string(id) + "].";
        lightShader->use();
        lightShader->setBool(name + "shadow_enabled", shadow_enabled);
        lightShader->setVec3(name + "position", object->transform.worldPosition());
        lightShader->setVec3(name + "direction", object->absForward());
        lightShader->setVec3(name + "ambient", ambient);
        lightShader->setVec3(name + "diffuse", diffuse);
        lightShader->setFloat(name + "innerCutOff", glm::cos(m_innerCutOff));
        lightShader->setFloat(name + "outerCutOff", glm::cos(m_outerCutOff));
        lightShader->setFloat(name + "linear", linear);
        lightShader->setFloat(name + "quadratic", quadratic);
        glActiveTexture(GL_TEXTURE0 + textureid);
        glBindTexture(GL_TEXTURE_2D, m_depthMap);
        lightShader->setMat4(name + "lightSpaceMatrix", m_lightSpaceMatrix);
        lightShader->setInt(name + "shadowMap", textureid);
    }

}
