#include "render/DirectionalLight.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include "core/GameObject.hpp"
#include "render/Model.hpp"

void DirectionalLight::setupShadowMap() {
    // framebuffer
    glGenFramebuffers(1, &depthMapFBO);
    // shadow map
    glGenTextures(1, &depthMap);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    // bind framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void DirectionalLight::genShadowMap(Shader *lightShader, int id, int textureid) {
    glm::vec3 absPos = this->object->absPos();

    GLint m_viewport[4];
    glGetIntegerv(GL_VIEWPORT, m_viewport);
    glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glClear(GL_DEPTH_BUFFER_BIT);
    glCullFace(GL_FRONT);

    this->depthShader->use();
    float nearPlane = 1.0f, farPlane = 7.5f;
    glm::mat4 lightProjection = glm::ortho(-20.0f, 20.0f, -20.0f, 20.0f, nearPlane, farPlane);
    glm::mat4 lightView = glm::lookAt(absPos, absPos + this->direction, glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 lightSpaceMatrix = lightProjection * lightView;
    this->depthShader->setMat4("lightSpaceMatrix", lightSpaceMatrix);
    for (Model *m : this->object->scene->getComponents<Model>(true)) {
        m->renderDepth(this->depthShader);
    }

    lightShader->use();
    glActiveTexture(GL_TEXTURE3 + textureid);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    lightShader->setMat4("dirLights[" + std::to_string(id) + "].lightSpaceMatrix", lightSpaceMatrix);
    lightShader->setInt("dirLights[" + std::to_string(id) + "].shadowMap", 3 + textureid);

    // reset framebuffer and viewport
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, m_viewport[2], m_viewport[3]);
    //glClear(GL_DEPTH_BUFFER_BIT);
    glCullFace(GL_BACK);
}