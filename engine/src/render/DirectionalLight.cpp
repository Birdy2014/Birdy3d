#include "render/DirectionalLight.hpp"

#include "core/RessourceManager.hpp"
#include "render/Camera.hpp"
#include "render/ModelComponent.hpp"
#include "render/Shader.hpp"
#include "scene/GameObject.hpp"
#include "scene/Scene.hpp"
#include <glm/gtc/matrix_transform.hpp>

namespace Birdy3d {

    DirectionalLight::DirectionalLight(glm::vec3 ambient, glm::vec3 diffuse, bool shadow_enabled)
        : Light(RessourceManager::getShader("directional_light_depth"), ambient, diffuse, 1.0f, 1.0f, shadow_enabled) {
        camOffset = 1000;
    }

    void DirectionalLight::setupShadowMap() {
        // framebuffer
        glGenFramebuffers(1, &m_depthMapFBO);
        // shadow map
        glGenTextures(1, &m_depthMap);
        glBindTexture(GL_TEXTURE_2D, m_depthMap);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
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
            genShadowMap();
            m_shadowMapUpdated = true;
        }
        std::string name = "dirLights[" + std::to_string(id) + "].";
        lightShader.use();
        lightShader.setBool(name + "shadow_enabled", shadow_enabled);
        lightShader.setVec3(name + "position", object->scene->m_current_camera->object->transform.worldPosition() - object->absForward() * camOffset);
        lightShader.setVec3(name + "direction", object->absForward());
        lightShader.setVec3(name + "ambient", ambient);
        lightShader.setVec3(name + "diffuse", diffuse);
        glActiveTexture(GL_TEXTURE0 + textureid);
        glBindTexture(GL_TEXTURE_2D, m_depthMap);
        lightShader.setMat4(name + "lightSpaceMatrix", lightSpaceMatrix);
        lightShader.setInt(name + "shadowMap", textureid);
        // TODO: cascaded shadow map
    }

    void DirectionalLight::genShadowMap() {
        glm::vec3 absPos = object->scene->m_current_camera->object->transform.worldPosition() - object->absForward() * camOffset;

        GLint viewport[4];
        glGetIntegerv(GL_VIEWPORT, viewport);
        glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
        glBindFramebuffer(GL_FRAMEBUFFER, m_depthMapFBO);
        glClear(GL_DEPTH_BUFFER_BIT);
        glCullFace(GL_FRONT);
        glEnable(GL_DEPTH_TEST);

        m_depthShader->use();
        float nearPlane = 1.0f, farPlane = 10000.0f;
        glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, nearPlane, farPlane);
        glm::mat4 lightView = glm::lookAt(absPos, absPos + object->absForward(), object->absUp());
        lightSpaceMatrix = lightProjection * lightView;
        m_depthShader->setMat4("lightSpaceMatrix", lightSpaceMatrix);
        for (ModelComponent* m : object->scene->getComponents<ModelComponent>(false, true)) {
            m->renderDepth(*m_depthShader);
        }

        // reset framebuffer and viewport
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, viewport[2], viewport[3]);
        //glClear(GL_DEPTH_BUFFER_BIT);
        glCullFace(GL_BACK);
    }

}
