#pragma once

#include "core/Base.hpp"
#include "core/RessourceManager.hpp"
#include "render/Texture.hpp"
#include "utils/serializer/Adapter.hpp"

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

        void serialize(serializer::Adapter&);

    private:
        std::string m_diffuse_map_name = "color::#ffffff";
        std::shared_ptr<Texture> m_diffuse_map = RessourceManager::get_color_texture(Color::WHITE);
        std::string m_specular_map_name = "color::#000000";
        std::shared_ptr<Texture> m_specular_map = RessourceManager::get_color_texture(Color::BLACK);
        std::string m_normal_map_name = "color::#ffffff";
        std::shared_ptr<Texture> m_normal_map = RessourceManager::get_color_texture(Color::WHITE);
        std::string m_emissive_map_name = "color::#000000";
        std::shared_ptr<Texture> m_emissive_map = RessourceManager::get_color_texture(Color::BLACK);

        BIRDY3D_REGISTER_TYPE_DEC(Material);
    };

}
