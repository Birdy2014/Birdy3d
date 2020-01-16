#include "Light.hpp"

void Light::setupShadowMap() {
    // framebuffer
    glGenFramebuffers(1, &depthMapFBO);
    // shadow map
    glGenTextures(1, &depthMap);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    float borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
    // bind framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Light::genShadowMap() {
    GLint m_viewport[4];
    glGetIntegerv(GL_VIEWPORT, m_viewport);
    glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glClear(GL_DEPTH_BUFFER_BIT);
    glCullFace(GL_FRONT);

    float nearPlane = 1.0f, farPlane = 7.5f;
    glm::mat4 lightProjection = glm::ortho(-20.0f, 20.0f, -20.0f, 20.0f, nearPlane, farPlane);
    glm::mat4 lightView = glm::lookAt(this->object->absPos(), this->object->absPos() + this->direction, glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 lightSpaceMatrix = lightProjection * lightView;

    this->object->depthShader->use();
    this->object->depthShader->setMat4("lightSpaceMatrix", lightSpaceMatrix);
    this->object->scene->renderDepth();
    this->object->shader->use();
    glActiveTexture(GL_TEXTURE10 + id);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    this->object->shader->setMat4("lightSpaceMatrix[" + std::to_string(id) + "]", lightSpaceMatrix);
    this->object->shader->setInt("lights[" + std::to_string(id) + "].shadowMap", 10 + id);

    // reset framebuffer and viewport
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, m_viewport[2], m_viewport[3]);
    glClear(GL_DEPTH_BUFFER_BIT);
    glCullFace(GL_BACK);
}