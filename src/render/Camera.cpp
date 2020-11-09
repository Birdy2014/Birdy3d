#include "Camera.hpp"
#include <glad/glad.h>
#include "Model.hpp"
#include "DirectionalLight.hpp"
#include "PointLight.hpp"

Camera::Camera(Shader *lightShader, int width, int height) {
    this->lightShader = lightShader;
    this->width = width;
    this->height = height;
}

Camera::Camera(Shader *lightShader, int width, int height, Widget *canvas) {
    this->lightShader = lightShader;
    this->width = width;
    this->height = height;
    this->canvas = canvas;
}

void Camera::start() {
    createGBuffer();
}

void Camera::resize(int width, int height) {
    if (this->width != width || this->height != height) {
        this->width = width;
        this->height = height;
        projectionMatrix = glm::perspective(glm::radians(80.0f), (float)width / (float)height, 0.1f, 100.0f);
        deleteGBuffer();
        createGBuffer();
    }
}

void Camera::render() {
    glm::vec3 absPos = this->object->absPos();
    glm::vec3 absForward = this->object->absForward();
    glm::vec3 right = this->object->absRight();
    glm::vec3 up = this->object->absUp();
    glm::mat4 view = glm::lookAt(absPos, absPos + absForward, up);

    // 1. geometry pass: render all geometric/color data to g-buffer
    glBindFramebuffer(GL_FRAMEBUFFER, this->gBuffer);
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    for (Model *m : this->object->scene->getComponents<Model>(true)) {
        m->object->shader->use();
        m->object->shader->setMat4("projection", this->projectionMatrix);
        m->object->shader->setMat4("view", view);
        m->render();
    }

    // 2. lighting pass
	glEnable(GL_BLEND);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gPosition);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, gNormal);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);
    int textureId = 0;
    std::vector<DirectionalLight*> dirLights = this->object->scene->getComponents<DirectionalLight>(true);
    std::vector<PointLight*> pointLights = this->object->scene->getComponents<PointLight>(true);
    for (int i = 0; i < dirLights.size(); i++)
        dirLights[i]->use(this->lightShader, i, textureId++);
    for (int i = 0; i < pointLights.size(); i++)
        pointLights[i]->use(this->lightShader, i, textureId++);

    this->lightShader->setVec3("viewPos", absPos);
    renderQuad();

    // GUI
    if (this->canvas) {
		glClear(GL_DEPTH_BUFFER_BIT);
        this->canvas->draw();
    }
	glDisable(GL_BLEND);
}

void Camera::createGBuffer() {
	glGenFramebuffers(1, &gBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);

	// - position color buffer
	glGenTextures(1, &gPosition);
	glBindTexture(GL_TEXTURE_2D, gPosition);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPosition, 0);

	// - normal color buffer
	glGenTextures(1, &gNormal);
	glBindTexture(GL_TEXTURE_2D, gNormal);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal, 0);

	// - color + specular color buffer
	glGenTextures(1, &gAlbedoSpec);
	glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gAlbedoSpec, 0);

	// - tell OpenGL which color attachments we'll use (of this framebuffer) for rendering
	unsigned int attachments[3] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2};
	glDrawBuffers(3, attachments);

	// create and attach depth buffer (renderbuffer)
	glGenRenderbuffers(1, &rboDepth);
	glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
	// finally check if framebuffer is complete
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Framebuffer not complete!" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Camera::deleteGBuffer() {
    unsigned int textures[3] = { gPosition, gNormal, gAlbedoSpec };
    glDeleteTextures(3, textures);
    glDeleteRenderbuffers(1, &this->rboDepth);
    glDeleteFramebuffers(1, &this->gBuffer);
}

void Camera::renderQuad() {
    if (this->quadVAO == 0) {
        float quadVertices[] = {
            // positions        // texture Coords
            -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
             1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
             1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        };
        // setup plane VAO
        glGenVertexArrays(1, &this->quadVAO);
        glGenBuffers(1, &this->quadVBO);
        glBindVertexArray(this->quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, this->quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    }
    glBindVertexArray(this->quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}
