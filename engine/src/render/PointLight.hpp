#pragma once

#include "render/Light.hpp"

namespace Birdy3d {

    class PointLight : public Light {
    public:
        glm::vec3 ambient;
        glm::vec3 diffuse;
        float linear;
        float quadratic;

        PointLight(glm::vec3 ambient = glm::vec3(0), glm::vec3 diffuse = glm::vec3(0), float linear = 0, float quadratic = 0, bool shadow_enabled = true);
        void setup_shadow_map() override;
        void gen_shadow_map() override;
        void use(const Shader& lightShader, int id, int textureid) override;

        template <class Archive>
        void serialize(Archive& ar) {
            ar(cereal::make_nvp("shadow_enabled", shadow_enabled));
            ar(cereal::make_nvp("ambient", ambient));
            ar(cereal::make_nvp("diffuse", diffuse));
            ar(cereal::make_nvp("linear", linear));
            ar(cereal::make_nvp("quadratic", quadratic));
            ar(cereal::make_nvp("far", m_far));
        }

    private:
        float m_far = 25.0f;
    };

}

CEREAL_REGISTER_TYPE(Birdy3d::PointLight);
CEREAL_REGISTER_POLYMORPHIC_RELATION(Birdy3d::Light, Birdy3d::PointLight);
