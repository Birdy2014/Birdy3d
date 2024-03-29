#include "render/PointLight.hpp"

#include "core/ResourceManager.hpp"
#include "ecs/Entity.hpp"
#include "ecs/Scene.hpp"
#include "render/ModelComponent.hpp"
#include "render/Shader.hpp"
#include <glm/gtc/matrix_transform.hpp>

namespace Birdy3d::render {

    PointLight::PointLight(utils::Color color, float intensity_ambient, float intensity_diffuse, float linear, float quadratic, bool shadow_enabled)
        : color(color)
        , intensity_ambient(intensity_ambient)
        , intensity_diffuse(intensity_diffuse)
        , linear(linear)
        , quadratic(quadratic)
        , shadow_enabled(shadow_enabled)
    { }

    void PointLight::setup_shadow_map()
    {
        m_depth_shader = core::ResourceManager::get_shader("point_light_depth.glsl");
        // framebuffer
        glGenFramebuffers(1, &m_shadow_map_fbo);
        // shadow map
        glGenTextures(1, &m_shadow_map);
        glBindTexture(GL_TEXTURE_CUBE_MAP, m_shadow_map);
        for (unsigned int i = 0; i < 6; i++)
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, shadow_width, shadow_height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        // bind framebuffer
        glBindFramebuffer(GL_FRAMEBUFFER, m_shadow_map_fbo);
        glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, m_shadow_map, 0);
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void PointLight::use(Shader const& light_shader, int id, int textureid)
    {
        if (!m_shadow_map_updated) {
            gen_shadow_map();
            m_shadow_map_updated = true;
        }
        std::string name = "point_lights[" + std::to_string(id) + "].";
        light_shader.use();
        light_shader.set_bool(name + "shadow_enabled", shadow_enabled);
        light_shader.set_vec3(name + "position", entity->transform.world_position());
        light_shader.set_vec3(name + "ambient", color.value * intensity_ambient);
        light_shader.set_vec3(name + "diffuse", color.value * intensity_diffuse);
        light_shader.set_float(name + "linear", linear);
        light_shader.set_float(name + "quadratic", quadratic);
        glActiveTexture(GL_TEXTURE0 + textureid);
        glBindTexture(GL_TEXTURE_CUBE_MAP, m_shadow_map);
        light_shader.set_int(name + "shadow_map", textureid);
        light_shader.set_float(name + "far", m_far);
    }

    void PointLight::gen_shadow_map()
    {
        glm::vec3 world_pos = entity->transform.world_position();

        glViewport(0, 0, shadow_width, shadow_height);
        glBindFramebuffer(GL_FRAMEBUFFER, m_shadow_map_fbo);
        glClear(GL_DEPTH_BUFFER_BIT);
        glCullFace(GL_FRONT);
        glEnable(GL_DEPTH_TEST);

        m_depth_shader->use();
        float aspect = (float)shadow_width / (float)shadow_height;
        float near = 1.0f;
        glm::mat4 shadow_proj = glm::perspective(glm::radians(90.0f), aspect, near, m_far);

        m_depth_shader->set_mat4("shadow_matrices[0]", shadow_proj * glm::lookAt(world_pos, world_pos + glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
        m_depth_shader->set_mat4("shadow_matrices[1]", shadow_proj * glm::lookAt(world_pos, world_pos + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
        m_depth_shader->set_mat4("shadow_matrices[2]", shadow_proj * glm::lookAt(world_pos, world_pos + glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0)));
        m_depth_shader->set_mat4("shadow_matrices[3]", shadow_proj * glm::lookAt(world_pos, world_pos + glm::vec3(0.0, -1.0, 0.0), glm::vec3(0.0, 0.0, -1.0)));
        m_depth_shader->set_mat4("shadow_matrices[4]", shadow_proj * glm::lookAt(world_pos, world_pos + glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, -1.0, 0.0)));
        m_depth_shader->set_mat4("shadow_matrices[5]", shadow_proj * glm::lookAt(world_pos, world_pos + glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.0, -1.0, 0.0)));
        m_depth_shader->set_float("far_plane", m_far);
        m_depth_shader->set_vec3("light_pos", world_pos);
        for (auto m : entity->scene->get_components<ModelComponent>(false, true)) {
            m->render_depth(*m_depth_shader);
        }

        glCullFace(GL_BACK);
    }

    void PointLight::start()
    {
        setup_shadow_map();
    }

    void PointLight::update()
    {
        if (shadow_enabled)
            m_shadow_map_updated = false;
    }

    void PointLight::serialize(serializer::Adapter& adapter)
    {
        adapter("shadow_enabled", shadow_enabled);
        adapter("color", color);
        adapter("intensity_ambient", intensity_ambient);
        adapter("intensity_diffuse", intensity_diffuse);
        adapter("linear", linear);
        adapter("quadratic", quadratic);
        adapter("far", m_far);
    }

    BIRDY3D_REGISTER_DERIVED_TYPE_DEF(ecs::Component, PointLight);

}
