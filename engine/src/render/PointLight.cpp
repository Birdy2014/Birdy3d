#include "render/PointLight.hpp"

#include "core/RessourceManager.hpp"
#include "render/ModelComponent.hpp"
#include "render/Shader.hpp"
#include "scene/GameObject.hpp"
#include "scene/Scene.hpp"
#include <glm/gtc/matrix_transform.hpp>

namespace Birdy3d {

    PointLight::PointLight(glm::vec3 ambient, glm::vec3 diffuse, float linear, float quadratic, bool shadow_enabled)
        : Light(shadow_enabled)
        , ambient(ambient)
        , diffuse(diffuse)
        , linear(linear)
        , quadratic(quadratic) { }

    void PointLight::setupShadowMap() {
        m_depthShader = RessourceManager::getShader("point_light_depth");
        // framebuffer
        glGenFramebuffers(1, &m_depthMapFBO);
        // shadow map
        glGenTextures(1, &m_depthMap);
        glBindTexture(GL_TEXTURE_CUBE_MAP, m_depthMap);
        for (unsigned int i = 0; i < 6; i++)
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
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
            genShadowMap();
            m_shadowMapUpdated = true;
        }
        std::string name = "pointLights[" + std::to_string(id) + "].";
        lightShader.use();
        lightShader.setBool(name + "shadow_enabled", shadow_enabled);
        lightShader.setVec3(name + "position", object->transform.worldPosition());
        lightShader.setVec3(name + "ambient", ambient);
        lightShader.setVec3(name + "diffuse", diffuse);
        lightShader.setFloat(name + "linear", linear);
        lightShader.setFloat(name + "quadratic", quadratic);
        glActiveTexture(GL_TEXTURE0 + textureid);
        glBindTexture(GL_TEXTURE_CUBE_MAP, m_depthMap);
        lightShader.setInt("pointLights[" + std::to_string(id) + "].shadowMap", textureid);
        lightShader.setFloat("pointLights[" + std::to_string(id) + "].far", m_far);
    }

    void PointLight::genShadowMap() {
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
        glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), aspect, near, m_far);

        std::vector<glm::mat4> shadowTransforms;
        shadowTransforms.push_back(shadowProj * glm::lookAt(absPos, absPos + glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
        shadowTransforms.push_back(shadowProj * glm::lookAt(absPos, absPos + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
        shadowTransforms.push_back(shadowProj * glm::lookAt(absPos, absPos + glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0)));
        shadowTransforms.push_back(shadowProj * glm::lookAt(absPos, absPos + glm::vec3(0.0, -1.0, 0.0), glm::vec3(0.0, 0.0, -1.0)));
        shadowTransforms.push_back(shadowProj * glm::lookAt(absPos, absPos + glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, -1.0, 0.0)));
        shadowTransforms.push_back(shadowProj * glm::lookAt(absPos, absPos + glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.0, -1.0, 0.0)));
        for (unsigned int i = 0; i < 6; i++)
            m_depthShader->setMat4("shadowMatrices[" + std::to_string(i) + "]", shadowTransforms[i]);
        m_depthShader->setFloat("far_plane", m_far);
        m_depthShader->setVec3("lightPos", absPos);
        for (auto m : object->scene->get_components<ModelComponent>(false, true)) {
            m->renderDepth(*m_depthShader);
        }

        // reset framebuffer and viewport
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, viewport[2], viewport[3]);
        glClear(GL_DEPTH_BUFFER_BIT);
        glCullFace(GL_BACK);
    }

}
