#include "render/Camera.hpp"

#include "core/Logger.hpp"
#include "core/RessourceManager.hpp"
#include "render/DirectionalLight.hpp"
#include "render/ModelComponent.hpp"
#include "render/PointLight.hpp"
#include "render/Shader.hpp"
#include "render/Spotlight.hpp"
#include "scene/GameObject.hpp"
#include "scene/Scene.hpp"
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <map>

namespace Birdy3d {

    Camera::Camera(int width, int height, bool deferred)
        : m_width(width)
        , m_height(height)
        , m_deferred_enabled(deferred) { }

    void Camera::start() {
        m_projection = glm::perspective(glm::radians(80.0f), (float)m_width / (float)m_height, 0.1f, 100.0f);
        createGBuffer();
        m_deferred_geometry_shader = RessourceManager::getShader("geometry_buffer");
        m_deferred_light_shader = RessourceManager::getShader("deferred_lighting");
        m_forward_shader = RessourceManager::getShader("forward_lighting");
        m_normal_shader = RessourceManager::getShader("normal_display");
        m_simple_color_shader = RessourceManager::getShader("simple_color");
        m_deferred_light_shader->use();
        m_deferred_light_shader->setInt("gPosition", 0);
        m_deferred_light_shader->setInt("gNormal", 1);
        m_deferred_light_shader->setInt("gAlbedoSpec", 2);
        // Set the default shadowmaps to the first texture in the right format so that the shader doesn't crash
        for (int i = 0; i < Shader::MAX_DIRECTIONAL_LIGHTS; i++) {
            m_deferred_light_shader->use();
            m_deferred_light_shader->setInt("dirLights[" + std::to_string(i) + "].shadowMap", 3);
            m_forward_shader->use();
            m_forward_shader->setInt("dirLights[" + std::to_string(i) + "].shadowMap", 4);
        }
        for (int i = 0; i < Shader::MAX_POINTLIGHTS; i++) {
            m_deferred_light_shader->use();
            m_deferred_light_shader->setInt("pointLights[" + std::to_string(i) + "].shadowMap", 3 + Shader::MAX_DIRECTIONAL_LIGHTS + i);
            m_forward_shader->use();
            m_forward_shader->setInt("pointLights[" + std::to_string(i) + "].shadowMap", 4 + Shader::MAX_DIRECTIONAL_LIGHTS + i);
        }
        for (int i = 0; i < Shader::MAX_SPOTLIGHTS; i++) {
            m_deferred_light_shader->use();
            m_deferred_light_shader->setInt("spotLights[" + std::to_string(i) + "].shadowMap", 3 + Shader::MAX_DIRECTIONAL_LIGHTS + Shader::MAX_POINTLIGHTS + i);
            m_forward_shader->use();
            m_forward_shader->setInt("spotLights[" + std::to_string(i) + "].shadowMap", 4 + Shader::MAX_DIRECTIONAL_LIGHTS + Shader::MAX_POINTLIGHTS + i);
        }
    }

    void Camera::cleanup() {
        deleteGBuffer();
        if (m_outline_vao != 0) {
            glDeleteVertexArrays(1, &m_outline_vao);
            glDeleteBuffers(1, &m_outline_vbo);
            m_outline_vao = 0;
            m_outline_vbo = 0;
        }
    }

    void Camera::resize(int width, int height) {
        if (m_width != width || m_height != height) {
            m_width = width;
            m_height = height;
            m_projection = glm::perspective(glm::radians(80.0f), (float)width / (float)height, 0.1f, 100.0f);
            deleteGBuffer(); // TODO: resize GBuffer instead of recreating it
            createGBuffer();
        }
    }

    void Camera::render() {
        object->scene->m_current_camera = this;
        glClearColor(0.0, 0.0, 0.0, 1.0);

        if (m_deferred_enabled) {
            renderDeferred();
            renderForward(false);
        } else {
            renderForward(true);
        }

        if (display_normals)
            renderNormals();
    }

    void Camera::createGBuffer() {
        glGenFramebuffers(1, &m_gbuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, m_gbuffer);

        // - position color buffer
        glGenTextures(1, &m_gbuffer_position);
        glBindTexture(GL_TEXTURE_2D, m_gbuffer_position);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, m_width, m_height, 0, GL_RGBA, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_gbuffer_position, 0);

        // - normal color buffer
        glGenTextures(1, &m_gbuffer_normal);
        glBindTexture(GL_TEXTURE_2D, m_gbuffer_normal);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, m_width, m_height, 0, GL_RGBA, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, m_gbuffer_normal, 0);

        // - color + specular color buffer
        glGenTextures(1, &m_gbuffer_albedo_spec);
        glBindTexture(GL_TEXTURE_2D, m_gbuffer_albedo_spec);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_width, m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, m_gbuffer_albedo_spec, 0);

        // - tell OpenGL which color attachments we'll use (of this framebuffer) for rendering
        unsigned int attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
        glDrawBuffers(3, attachments);

        // create and attach depth buffer (renderbuffer)
        glGenRenderbuffers(1, &m_rbo_depth);
        glBindRenderbuffer(GL_RENDERBUFFER, m_rbo_depth);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, m_width, m_height);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_rbo_depth);
        // finally check if framebuffer is complete
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            Logger::error("Framebuffer not complete!");
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void Camera::deleteGBuffer() {
        unsigned int textures[3] = { m_gbuffer_position, m_gbuffer_normal, m_gbuffer_albedo_spec };
        glDeleteTextures(3, textures);
        glDeleteRenderbuffers(1, &m_rbo_depth);
        glDeleteFramebuffers(1, &m_gbuffer);
    }

    void Camera::renderQuad() {
        if (m_quad_vao == 0) {
            float quadVertices[] = {
                // clang-format off
                // positions        // texture Coords
                -1.0f, 1.0f,  0.0f, 0.0f, 1.0f,
                -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
                1.0f,  1.0f,  0.0f, 1.0f, 1.0f,
                1.0f,  -1.0f, 0.0f, 1.0f, 0.0f,
                // clang-format on
            };
            // setup plane VAO
            glGenVertexArrays(1, &m_quad_vao);
            glGenBuffers(1, &m_quad_vbo);
            glBindVertexArray(m_quad_vao);
            glBindBuffer(GL_ARRAY_BUFFER, m_quad_vbo);
            glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
            glEnableVertexAttribArray(1);
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
        }
        glBindVertexArray(m_quad_vao);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glBindVertexArray(0);
    }

    void Camera::renderDeferred() {
        glm::vec3 absPos = object->transform.worldPosition();
        glm::vec3 absForward = object->absForward();
        glm::vec3 up = object->absUp();
        glm::mat4 view = glm::lookAt(absPos, absPos + absForward, up);

        // 1. geometry pass: render all geometric/color data to g-buffer
        glEnable(GL_DEPTH_TEST);
        glDisable(GL_BLEND);
        glBindFramebuffer(GL_FRAMEBUFFER, m_gbuffer);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        for (ModelComponent* m : object->scene->getComponents<ModelComponent>(false, true)) {
            m_deferred_geometry_shader->use();
            m_deferred_geometry_shader->setMat4("projection", m_projection);
            m_deferred_geometry_shader->setMat4("view", view);
            m->render(*m_deferred_geometry_shader, false);
        }

        // 2. lighting pass
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClear(GL_COLOR_BUFFER_BIT);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_gbuffer_position);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, m_gbuffer_normal);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, m_gbuffer_albedo_spec);
        std::vector<DirectionalLight*> dirLights = object->scene->getComponents<DirectionalLight>(false, true);
        std::vector<PointLight*> pointLights = object->scene->getComponents<PointLight>(false, true);
        std::vector<Spotlight*> spotlights = object->scene->getComponents<Spotlight>(false, true);
        m_deferred_light_shader->use();
        m_deferred_light_shader->setInt("nr_directional_lights", dirLights.size());
        m_deferred_light_shader->setInt("nr_pointlights", pointLights.size());
        m_deferred_light_shader->setInt("nr_spotlights", spotlights.size());
        for (size_t i = 0; i < dirLights.size(); i++)
            dirLights[i]->use(*m_deferred_light_shader, i, 3 + i);
        for (size_t i = 0; i < pointLights.size(); i++)
            pointLights[i]->use(*m_deferred_light_shader, i, 3 + Shader::MAX_DIRECTIONAL_LIGHTS + i);
        for (size_t i = 0; i < spotlights.size(); i++)
            spotlights[i]->use(*m_deferred_light_shader, i, 3 + Shader::MAX_DIRECTIONAL_LIGHTS + Shader::MAX_POINTLIGHTS + i);

        m_deferred_light_shader->setVec3("viewPos", absPos);
        renderQuad();
    }

    void Camera::renderForward(bool renderOpaque) {
        glm::vec3 absPos = object->transform.worldPosition();
        glm::vec3 absForward = object->absForward();
        glm::vec3 up = object->absUp();
        glm::mat4 view = glm::lookAt(absPos, absPos + absForward, up);

        glEnable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        std::vector<DirectionalLight*> dirLights = object->scene->getComponents<DirectionalLight>(false, true);
        std::vector<PointLight*> pointLights = object->scene->getComponents<PointLight>(false, true);
        std::vector<Spotlight*> spotlights = object->scene->getComponents<Spotlight>(false, true);
        m_forward_shader->use();
        m_forward_shader->setInt("nr_directional_lights", dirLights.size());
        m_forward_shader->setInt("nr_pointlights", pointLights.size());
        m_forward_shader->setInt("nr_spotlights", spotlights.size());
        for (size_t i = 0; i < dirLights.size(); i++)
            dirLights[i]->use(*m_forward_shader, i, 4 + i);
        for (size_t i = 0; i < pointLights.size(); i++)
            pointLights[i]->use(*m_forward_shader, i, 4 + Shader::MAX_DIRECTIONAL_LIGHTS + i);
        for (size_t i = 0; i < spotlights.size(); i++)
            spotlights[i]->use(*m_forward_shader, i, 4 + Shader::MAX_DIRECTIONAL_LIGHTS + Shader::MAX_POINTLIGHTS + i);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        if (renderOpaque) {
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        } else {
            glBindFramebuffer(GL_READ_FRAMEBUFFER, m_gbuffer);
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
            glBlitFramebuffer(0, 0, m_width, m_height, 0, 0, m_width, m_height, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
        }

        m_forward_shader->setMat4("projection", m_projection);
        m_forward_shader->setMat4("view", view);
        m_forward_shader->setVec3("viewPos", absPos);
        if (renderOpaque) {
            for (ModelComponent* m : object->scene->getComponents<ModelComponent>(false, true)) {
                m->render(*m_forward_shader, false);
            }
        }

        // Transparency
        std::vector<ModelComponent*> models = object->scene->getComponents<ModelComponent>(false, true);
        std::map<float, ModelComponent*> sorted;
        for (ModelComponent* m : models) {
            float distance = glm::length(object->transform.position - m->object->transform.position);
            sorted[distance] = m;
        }

        for (std::map<float, ModelComponent*>::reverse_iterator it = sorted.rbegin(); it != sorted.rend(); it++) {
            it->second->render(*m_forward_shader, true);
        }
    }

    void Camera::renderNormals() {
        glm::vec3 absPos = object->transform.worldPosition();
        glm::vec3 absForward = object->absForward();
        glm::vec3 up = object->absUp();
        glm::mat4 view = glm::lookAt(absPos, absPos + absForward, up);

        glEnable(GL_DEPTH_TEST);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        m_normal_shader->use();
        m_normal_shader->setMat4("projection", m_projection);
        m_normal_shader->setMat4("view", view);
        for (ModelComponent* m : object->scene->getComponents<ModelComponent>(false, true)) {
            m->render(*m_normal_shader, false);
            m->render(*m_normal_shader, true);
        }
    }

    void Camera::renderOutline(const GameObject* selected_object) {
        if (selected_object == nullptr)
            return;

        if (m_outline_vao == 0) {
            glGenVertexArrays(1, &m_outline_vao);
            glGenBuffers(1, &m_outline_vbo);

            glBindVertexArray(m_outline_vao);
            glBindBuffer(GL_ARRAY_BUFFER, m_outline_vbo);

            glm::vec3 vertices[24];
            glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices[0], GL_STATIC_DRAW);

            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
        }

        const float outline_offset = 0.4;

        glm::vec3 low(std::numeric_limits<float>::infinity());
        glm::vec3 high(-std::numeric_limits<float>::infinity());

        std::pair<glm::vec3, glm::vec3> bounding_box;
        glm::mat4 model;
        for (ModelComponent* model_component : selected_object->getComponents<ModelComponent>(false, true)) {
            model = model_component->object->transform.matrix();
            bounding_box = model_component->model->bounding_box();
            glm::vec3 model_low = model * glm::vec4(bounding_box.first, 1.0f);
            glm::vec3 model_high = model * glm::vec4(bounding_box.second, 1.0f);
            if (model_low.x < low.x)
                low.x = model_low.x;
            if (model_low.y < low.y)
                low.y = model_low.y;
            if (model_low.z < low.z)
                low.z = model_low.z;
            if (model_high.x > high.x)
                high.x = model_high.x;
            if (model_high.y > high.y)
                high.y = model_high.y;
            if (model_high.z > high.z)
                high.z = model_high.z;
        }

        low -= outline_offset;
        high += outline_offset;

        // clang-format off
        glm::vec3 vertices[24] = {
            // Bottom rectangle
            glm::vec3(low.x, low.y, low.z), glm::vec3(high.x, low.y, low.z),
            glm::vec3(high.x, low.y, low.z), glm::vec3(high.x, low.y, high.z),
            glm::vec3(high.x, low.y, high.z), glm::vec3(low.x, low.y, high.z),
            glm::vec3(low.x, low.y, high.z), glm::vec3(low.x, low.y, low.z),
            // Top rectangle
            glm::vec3(low.x, high.y, low.z), glm::vec3(high.x, high.y, low.z),
            glm::vec3(high.x, high.y, low.z), glm::vec3(high.x, high.y, high.z),
            glm::vec3(high.x, high.y, high.z), glm::vec3(low.x, high.y, high.z),
            glm::vec3(low.x, high.y, high.z), glm::vec3(low.x, high.y, low.z),
            // Side rectangles
            glm::vec3(low.x, low.y, low.z), glm::vec3(low.x, high.y, low.z),
            glm::vec3(high.x, low.y, low.z), glm::vec3(high.x, high.y, low.z),
            glm::vec3(high.x, low.y, high.z), glm::vec3(high.x, high.y, high.z),
            glm::vec3(low.x, low.y, high.z), glm::vec3(low.x, high.y, high.z),
        };
        // clang-format on
        glBindVertexArray(m_outline_vao);
        glBindBuffer(GL_ARRAY_BUFFER, m_outline_vbo);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), &vertices[0]);

        glm::vec3 absPos = object->transform.worldPosition();
        glm::vec3 absForward = object->absForward();
        glm::vec3 up = object->absUp();
        glm::mat4 view = glm::lookAt(absPos, absPos + absForward, up);

        glEnable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClear(GL_DEPTH_BUFFER_BIT);

        m_simple_color_shader->use();
        m_simple_color_shader->setMat4("projection", m_projection);
        m_simple_color_shader->setMat4("view", view);
        m_simple_color_shader->setVec4("color", Color("#e0902180"));
        m_simple_color_shader->setMat4("model", glm::mat4(1));
        glBindVertexArray(m_outline_vao);
        glDrawArrays(GL_LINES, 0, 24);
    }

}
