#pragma once

#include "render/Light.hpp"

namespace Birdy3d {

    class Spotlight : public Light {
    public:
        glm::vec3 ambient;
        glm::vec3 diffuse;
        float linear;
        float quadratic;

        Spotlight(glm::vec3 ambient = glm::vec3(0), glm::vec3 diffuse = glm::vec3(0), float innerCutOff = 0, float outerCutOff = 0, float linear = 0, float quadratic = 0, bool shadow_enabled = true);
        void setupShadowMap() override;
        void genShadowMap() override;
        void use(const Shader& lightShader, int id, int textureid) override;

        template <class Archive>
        void serialize(Archive& ar) {
            ar(cereal::make_nvp("shadow_enabled", shadow_enabled));
            ar(cereal::make_nvp("ambient", ambient));
            ar(cereal::make_nvp("diffuse", diffuse));
            ar(cereal::make_nvp("linear", linear));
            ar(cereal::make_nvp("quadratic", quadratic));
            ar(cereal::make_nvp("inner_cutoff", m_innerCutOff));
            ar(cereal::make_nvp("outer_cutoff", m_outerCutOff));
            ar(cereal::make_nvp("far", m_far));
        }

    private:
        float m_far = 25.0f;
        glm::mat4 m_lightSpaceMatrix;
        float m_innerCutOff;
        float m_outerCutOff;
    };

}

CEREAL_REGISTER_TYPE(Birdy3d::Spotlight);
CEREAL_REGISTER_POLYMORPHIC_RELATION(Birdy3d::Light, Birdy3d::Spotlight);
