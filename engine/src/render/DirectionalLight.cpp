#include "render/DirectionalLight.hpp"

#include "core/GameObject.hpp"
#include "core/RessourceManager.hpp"
#include "render/ModelComponent.hpp"
#include "render/Shader.hpp"
#include <glm/gtc/matrix_transform.hpp>

namespace Birdy3d {

    DirectionalLight::DirectionalLight(glm::vec3 ambient, glm::vec3 diffuse)
        : Light() {
        this->depthShader = RessourceManager::getShader("directional_light_depth");
        this->ambient = ambient;
        this->diffuse = diffuse;
    }

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

    void DirectionalLight::use(Shader* lightShader, int id, int textureid) {
        if (!shadowMapUpdated) {
            genShadowMap(lightShader, id, textureid);
            shadowMapUpdated = true;
        }
        std::string name = "dirLights[" + std::to_string(id) + "].";
        lightShader->use();
        lightShader->setVec3(name + "position", this->object->transform.worldPosition());
        lightShader->setVec3(name + "direction", this->object->absForward());
        lightShader->setVec3(name + "ambient", ambient);
        lightShader->setVec3(name + "diffuse", diffuse);
        glActiveTexture(GL_TEXTURE0 + textureid);
        glBindTexture(GL_TEXTURE_2D, depthMap);
        lightShader->setMat4(name + "lightSpaceMatrix", lightSpaceMatrix);
        lightShader->setInt(name + "shadowMap", textureid);
    }

    void DirectionalLight::genShadowMap(Shader* lightShader, int id, int textureid) {
        glm::vec3 absPos = this->object->transform.worldPosition();

        GLint m_viewport[4];
        glGetIntegerv(GL_VIEWPORT, m_viewport);
        glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
        glClear(GL_DEPTH_BUFFER_BIT);
        glCullFace(GL_FRONT);
        glEnable(GL_DEPTH_TEST);

        this->depthShader->use();
        float nearPlane = 1.0f, farPlane = 7.5f;
        glm::mat4 lightProjection = glm::ortho(-20.0f, 20.0f, -20.0f, 20.0f, nearPlane, farPlane);
        glm::mat4 lightView = glm::lookAt(absPos, absPos + this->object->absForward(), glm::vec3(0.0f, 1.0f, 0.0f));
        lightSpaceMatrix = lightProjection * lightView;
        this->depthShader->setMat4("lightSpaceMatrix", lightSpaceMatrix);
        for (ModelComponent* m : this->object->scene->getComponents<ModelComponent>(false, true)) {
            m->renderDepth(this->depthShader);
        }

        // reset framebuffer and viewport
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, m_viewport[2], m_viewport[3]);
        //glClear(GL_DEPTH_BUFFER_BIT);
        glCullFace(GL_BACK);
    }

}
