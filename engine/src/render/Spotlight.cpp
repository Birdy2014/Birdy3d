#include "render/Spotlight.hpp"

#include "core/ResourceManager.hpp"
#include "ecs/Entity.hpp"
#include "ecs/Scene.hpp"
#include "render/ModelComponent.hpp"
#include "render/Shader.hpp"
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>

namespace Birdy3d::render {

    Spotlight::Spotlight(glm::vec3 ambient, glm::vec3 diffuse, float innerCutOff, float outerCutOff, float linear, float quadratic, bool shadow_enabled)
        : ambient(ambient)
        , diffuse(diffuse)
        , linear(linear)
        , quadratic(quadratic)
        , shadow_enabled(shadow_enabled)
        , m_inner_cutoff(innerCutOff)
        , m_outer_cutoff(outerCutOff) { }

    void Spotlight::setup_shadow_map() {
        m_depth_shader = core::ResourceManager::get_shader("directional_light_depth");
        // framebuffer
        glGenFramebuffers(1, &m_shadow_map_fbo);
        // shadow map
        glGenTextures(1, &m_shadow_map);
        glBindTexture(GL_TEXTURE_2D, m_shadow_map);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        // bind framebuffer
        glBindFramebuffer(GL_FRAMEBUFFER, m_shadow_map_fbo);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_shadow_map, 0);
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void Spotlight::gen_shadow_map() {
        glm::vec3 world_pos = entity->transform.world_position();

        GLint viewport[4];
        glGetIntegerv(GL_VIEWPORT, viewport);
        glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
        glBindFramebuffer(GL_FRAMEBUFFER, m_shadow_map_fbo);
        glClear(GL_DEPTH_BUFFER_BIT);
        glCullFace(GL_FRONT);
        glEnable(GL_DEPTH_TEST);

        m_depth_shader->use();
        float aspect = (float)SHADOW_WIDTH / (float)SHADOW_HEIGHT;
        float near = 1.0f;
        m_far = 25.0f;
        glm::mat4 light_projection = glm::perspective(m_outer_cutoff * 2, aspect, near, m_far);
        glm::mat4 light_view = glm::lookAt(world_pos, world_pos + entity->world_forward(), entity->world_up());

        m_light_space_transform = light_projection * light_view;
        m_depth_shader->set_mat4("lightSpaceMatrix", m_light_space_transform);
        for (auto m : entity->scene->get_components<ModelComponent>(false, true)) {
            m->render_depth(*m_depth_shader);
        }

        // reset framebuffer and viewport
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, viewport[2], viewport[3]);
        // glClear(GL_DEPTH_BUFFER_BIT);
        glCullFace(GL_BACK);
    }

    void Spotlight::use(const Shader& lightShader, int id, int textureid) {
        if (!m_shadow_map_updated) {
            gen_shadow_map();
            m_shadow_map_updated = true;
        }
        std::string name = "spotlights[" + std::to_string(id) + "].";
        lightShader.use();
        lightShader.set_bool(name + "shadow_enabled", shadow_enabled);
        lightShader.set_vec3(name + "position", entity->transform.world_position());
        lightShader.set_vec3(name + "direction", entity->world_forward());
        lightShader.set_vec3(name + "ambient", ambient);
        lightShader.set_vec3(name + "diffuse", diffuse);
        lightShader.set_float(name + "innerCutOff", glm::cos(m_inner_cutoff));
        lightShader.set_float(name + "outerCutOff", glm::cos(m_outer_cutoff));
        lightShader.set_float(name + "linear", linear);
        lightShader.set_float(name + "quadratic", quadratic);
        glActiveTexture(GL_TEXTURE0 + textureid);
        glBindTexture(GL_TEXTURE_2D, m_shadow_map);
        lightShader.set_mat4(name + "lightSpaceMatrix", m_light_space_transform);
        lightShader.set_int(name + "shadowMap", textureid);
    }

    void Spotlight::start() {
        setup_shadow_map();
    }

    void Spotlight::update() {
        if (shadow_enabled)
            m_shadow_map_updated = false;
    }

    void Spotlight::serialize(serializer::Adapter& adapter) {
        adapter("shadow_enabled", shadow_enabled);
        adapter("ambient", ambient);
        adapter("diffuse", diffuse);
        adapter("linear", linear);
        adapter("quadratic", quadratic);
        adapter("inner_cutoff", m_inner_cutoff);
        adapter("outer_cutoff", m_outer_cutoff);
        adapter("far", m_far);
    }

    BIRDY3D_REGISTER_DERIVED_TYPE_DEF(ecs::Component, Spotlight);

}
