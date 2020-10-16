#include "Light.hpp"
#include "Model.hpp"

void Light::use(Shader *shader, int id) {
    std::string i = std::to_string(id);
    shader->setInt("lights[" + i + "].type", type);
    shader->setVec3("lights[" + i + "].position", this->object->absPos());
    shader->setVec3("lights[" + i + "].direction", direction);
    shader->setVec3("lights[" + i + "].ambient", ambient);
    shader->setVec3("lights[" + i + "].diffuse", diffuse);
    shader->setVec3("lights[" + i + "].specular", specular);
    shader->setFloat("lights[" + i + "].linear", linear);
    shader->setFloat("lights[" + i + "].quadratic", quadratic);
    shader->setFloat("lights[" + i + "].innerCutOff", innerCutOff);
    shader->setFloat("lights[" + i + "].outerCutOff", outerCutOff);
    genShadowMap(shader, id);
}

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

void Light::genShadowMap(Shader *lightShader, int id) {
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
    for (Model *m : this->object->scene->getComponents<Model>()) {
        m->renderDepth();
    }
    lightShader->use();
    glActiveTexture(GL_TEXTURE3 + id);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    lightShader->setMat4("lights[" + std::to_string(id) + "].lightSpaceMatrix", lightSpaceMatrix);
    lightShader->setInt("lights[" + std::to_string(id) + "].shadowMap", 3 + id);

    // reset framebuffer and viewport
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, m_viewport[2], m_viewport[3]);
    glClear(GL_DEPTH_BUFFER_BIT);
    glCullFace(GL_BACK);
}