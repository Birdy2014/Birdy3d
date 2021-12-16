#pragma once

#include "core/Base.hpp"
#include "core/ResourceManager.hpp"
#include "render/Texture.hpp"
#include "utils/serializer/Adapter.hpp"

namespace Birdy3d::render {

    class Material {
    public:
        bool diffuse_map_enabled = false;
        utils::Color diffuse_color = utils::Color::WHITE;
        void diffuse_map(const std::string&);

        bool specular_map_enabled = false;
        float specular_value = 0.0f;
        void specular_map(const std::string&);

        bool normal_map_enabled = false;
        void normal_map(const std::string&);

        bool emissive_map_enabled = false;
        utils::Color emissive_color = utils::Color::BLACK;
        void emissive_map(const std::string&);

        void use(const Shader& shader) const;
        bool transparent() const;

        void serialize(serializer::Adapter&);

    private:
        std::string m_diffuse_map_name = "color::#ffffff";
        core::ResourceHandle<Texture> m_diffuse_map = core::ResourceManager::get_texture("color::" + utils::Color::WHITE.to_string());
        std::string m_specular_map_name = "color::#000000";
        core::ResourceHandle<Texture> m_specular_map = core::ResourceManager::get_texture("color::" + utils::Color::BLACK.to_string());
        std::string m_normal_map_name = "color::#ffffff";
        core::ResourceHandle<Texture> m_normal_map = core::ResourceManager::get_texture("color::" + utils::Color::WHITE.to_string());
        std::string m_emissive_map_name = "color::#000000";
        core::ResourceHandle<Texture> m_emissive_map = core::ResourceManager::get_texture("color::" + utils::Color::BLACK.to_string());

        BIRDY3D_REGISTER_TYPE_DEC(Material);
    };

}
