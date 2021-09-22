#pragma once

#include "render/Light.hpp"

namespace Birdy3d {

    class DirectionalLight : public Light {
    public:
        glm::vec3 ambient;
        glm::vec3 diffuse;

        DirectionalLight(glm::vec3 ambient = glm::vec3(0), glm::vec3 diffuse = glm::vec3(0), bool shadow_enabled = true);
        void setup_shadow_map() override;
        void gen_shadow_map() override;
        void use(const Shader& lightShader, int id, int textureid) override;

        template <class Archive>
        void serialize(Archive& ar) {
            ar(cereal::make_nvp("shadow_enabled", shadow_enabled));
            ar(cereal::make_nvp("ambient", ambient));
            ar(cereal::make_nvp("diffuse", diffuse));
            ar(cereal::make_nvp("cam_offset", m_cam_offset));
        }

    private:
        float m_cam_offset;
        glm::mat4 m_light_space_matrix;
    };

}

CEREAL_REGISTER_TYPE(Birdy3d::DirectionalLight);
CEREAL_REGISTER_POLYMORPHIC_RELATION(Birdy3d::Light, Birdy3d::DirectionalLight);
