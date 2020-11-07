#include "PointLight.hpp"
#include "Model.hpp"

void PointLight::setupShadowMap() {
    // framebuffer
    glGenFramebuffers(1, &depthMapFBO);
    // shadow map
    glGenTextures(1, &depthMap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, depthMap);
    for (unsigned int i = 0; i < 6; i++)
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    //float borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
    //glTexParameterfv(texture_type, GL_TEXTURE_BORDER_COLOR, borderColor);
    // bind framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthMap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void PointLight::genShadowMap(Shader *lightShader, int id, int textureid) {
    glm::vec3 absPos = this->object->absPos();

    GLint m_viewport[4];
    glGetIntegerv(GL_VIEWPORT, m_viewport);
    glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glClear(GL_DEPTH_BUFFER_BIT);
    glCullFace(GL_FRONT);

    this->depthShader->use();
    float aspect = (float)SHADOW_WIDTH / (float)SHADOW_HEIGHT;
    float near = 1.0f;
    float far = 25.0f;
    glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), aspect, near, far);

    std::vector<glm::mat4> shadowTransforms;
    shadowTransforms.push_back(shadowProj * glm::lookAt(absPos, absPos + glm::vec3( 1.0, 0.0, 0.0), glm::vec3(0.0,-1.0, 0.0)));
    shadowTransforms.push_back(shadowProj * glm::lookAt(absPos, absPos + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0,-1.0, 0.0)));
    shadowTransforms.push_back(shadowProj * glm::lookAt(absPos, absPos + glm::vec3( 0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0)));
    shadowTransforms.push_back(shadowProj * glm::lookAt(absPos, absPos + glm::vec3( 0.0,-1.0, 0.0), glm::vec3(0.0, 0.0,-1.0)));
    shadowTransforms.push_back(shadowProj * glm::lookAt(absPos, absPos + glm::vec3( 0.0, 0.0, 1.0), glm::vec3(0.0,-1.0, 0.0)));
    shadowTransforms.push_back(shadowProj * glm::lookAt(absPos, absPos + glm::vec3( 0.0, 0.0,-1.0), glm::vec3(0.0,-1.0, 0.0)));
    for (unsigned int i = 0; i < 6; i++)
        this->depthShader->setMat4("shadowMatrices[" + std::to_string(i) + "]", shadowTransforms[i]);
    this->depthShader->setFloat("far_plane", far);
    this->depthShader->setVec3("lightPos", absPos);
    for (Model *m : this->object->scene->getComponents<Model>(true)) {
        m->renderDepth(this->depthShader);
    }

    lightShader->use();
    glActiveTexture(GL_TEXTURE3 + textureid);
    glBindTexture(GL_TEXTURE_CUBE_MAP, depthMap);
    lightShader->setInt("pointLights[" + std::to_string(id) + "].shadowMap", 3 + textureid);
    lightShader->setFloat("pointLights[" + std::to_string(id) + "].far", far);

    // reset framebuffer and viewport
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, m_viewport[2], m_viewport[3]);
    glClear(GL_DEPTH_BUFFER_BIT);
    glCullFace(GL_BACK);
}