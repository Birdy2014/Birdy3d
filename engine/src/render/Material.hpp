#pragma once

#include "core/Base.hpp"
#include "core/RessourceManager.hpp"
#include "render/Texture.hpp"

namespace Birdy3d {

    class Material {
    public:
        bool diffuse_map_enabled = false;
        Color diffuse_color = Color::WHITE;
        void diffuse_map(const std::string&);

        bool specular_map_enabled = false;
        float specular_value = 0.0f;
        void specular_map(const std::string&);

        bool normal_map_enabled = false;
        void normal_map(const std::string&);

        bool emissive_map_enabled = false;
        Color emissive_color = Color::BLACK;
        void emissive_map(const std::string&);

        void use(const Shader& shader) const;
        bool transparent() const;

        template <class Archive>
        void save(Archive& ar) const {
            ar(CEREAL_NVP(diffuse_map_enabled));
            ar(CEREAL_NVP(diffuse_color));
            ar(cereal::make_nvp("diffuse_map", m_diffuse_map_name));

            ar(CEREAL_NVP(specular_map_enabled));
            ar(CEREAL_NVP(specular_value));
            ar(cereal::make_nvp("specular_map", m_specular_map_name));

            ar(CEREAL_NVP(normal_map_enabled));
            ar(cereal::make_nvp("normal_map", m_normal_map_name));

            ar(CEREAL_NVP(emissive_map_enabled));
            ar(CEREAL_NVP(emissive_color));
            ar(cereal::make_nvp("emissive_map", m_emissive_map_name));
        }

        template <class Archive>
        void load(Archive& ar) {
            ar(CEREAL_NVP(diffuse_map_enabled));
            ar(CEREAL_NVP(diffuse_color));
            std::string diffuse;
            ar(cereal::make_nvp("diffuse_map", diffuse));
            diffuse_map(diffuse);

            ar(CEREAL_NVP(specular_map_enabled));
            ar(CEREAL_NVP(specular_value));
            std::string specular;
            ar(cereal::make_nvp("specular_map", specular));
            specular_map(specular);

            ar(CEREAL_NVP(normal_map_enabled));
            std::string normal;
            ar(cereal::make_nvp("normal_map", normal));
            normal_map(normal);

            ar(CEREAL_NVP(emissive_map_enabled));
            ar(CEREAL_NVP(emissive_color));
            std::string emissive;
            ar(cereal::make_nvp("emissive_map", emissive));
            emissive_map(emissive);
        }

    private:
        std::string m_diffuse_map_name = "color::#ffffff";
        std::shared_ptr<Texture> m_diffuse_map = RessourceManager::getColorTexture(Color::WHITE);
        std::string m_specular_map_name = "color::#000000";
        std::shared_ptr<Texture> m_specular_map = RessourceManager::getColorTexture(Color::BLACK);
        std::string m_normal_map_name = "color::#ffffff";
        std::shared_ptr<Texture> m_normal_map = RessourceManager::getColorTexture(Color::WHITE);
        std::string m_emissive_map_name = "color::#000000";
        std::shared_ptr<Texture> m_emissive_map = RessourceManager::getColorTexture(Color::BLACK);
    };

}
