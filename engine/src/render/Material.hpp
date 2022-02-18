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
        void diffuse_map(const core::ResourceIdentifier&);

        bool specular_map_enabled = false;
        float specular_value = 0.0f;
        void specular_map(const core::ResourceIdentifier&);

        bool normal_map_enabled = false;
        void normal_map(const core::ResourceIdentifier&);

        bool emissive_map_enabled = false;
        utils::Color emissive_color = utils::Color::BLACK;
        void emissive_map(const core::ResourceIdentifier&);

        void use(const Shader& shader) const;
        bool transparent() const;

        void serialize(serializer::Adapter&);

    private:
        core::ResourceHandle<Texture> m_diffuse_map = core::ResourceManager::get_texture("color::" + utils::Color::WHITE.to_string());
        core::ResourceHandle<Texture> m_specular_map = core::ResourceManager::get_texture("color::" + utils::Color::BLACK.to_string());
        core::ResourceHandle<Texture> m_normal_map = core::ResourceManager::get_texture("color::" + utils::Color::WHITE.to_string());
        core::ResourceHandle<Texture> m_emissive_map = core::ResourceManager::get_texture("color::" + utils::Color::BLACK.to_string());

        BIRDY3D_REGISTER_TYPE_DEC(Material);
    };

}
