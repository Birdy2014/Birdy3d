#include "render/Material.hpp"

#include "render/Shader.hpp"

namespace Birdy3d::render {

    void Material::diffuse_map(const std::string& name) {
        m_diffuse_map_name = name;
        m_diffuse_map = core::ResourceManager::get_texture(name);
    }

    void Material::specular_map(const std::string& name) {
        m_specular_map_name = name;
        m_specular_map = core::ResourceManager::get_texture(name);
    }

    void Material::normal_map(const std::string& name) {
        m_normal_map_name = name;
        m_normal_map = core::ResourceManager::get_texture(name);
    }

    void Material::emissive_map(const std::string& name) {
        m_emissive_map_name = name;
        m_emissive_map = core::ResourceManager::get_texture(name);
    }

    void Material::use(const Shader& shader) const {
        shader.set_bool("material.diffuse_map_enabled", diffuse_map_enabled);
        shader.set_vec4("material.diffuse_color", diffuse_color);
        m_diffuse_map->bind(0);
        shader.set_int("material.diffuse_map", 0);

        shader.set_bool("material.specular_map_enabled", specular_map_enabled);
        shader.set_float("material.specular_value", specular_value);
        m_specular_map->bind(1);
        shader.set_int("material.specular_map", 1);

        shader.set_bool("material.normal_map_enabled", normal_map_enabled);
        m_normal_map->bind(2);
        shader.set_int("material.normal_map", 2);

        shader.set_bool("material.emissive_map_enabled", emissive_map_enabled);
        shader.set_vec4("material.emissive_color", emissive_color);
        m_emissive_map->bind(3);
        shader.set_int("material.emissive_map", 3);
    }

    bool Material::transparent() const {
        if (diffuse_map_enabled)
            return m_diffuse_map->transparent();
        else
            return diffuse_color.value.a < 1;
    }

    void Material::serialize(serializer::Adapter& adapter) {
        bool loading = adapter.mode() == serializer::Adapter::Mode::LOAD;

        adapter("diffuse_map_enabled", diffuse_map_enabled);
        adapter("diffuse_color", diffuse_color);
        adapter("diffuse_map", m_diffuse_map_name);
        if (loading)
            diffuse_map(m_diffuse_map_name);

        adapter("specular_map_enabled", specular_map_enabled);
        adapter("specular_value", specular_value);
        adapter("specular_map", m_specular_map_name);
        if (loading)
            specular_map(m_specular_map_name);

        adapter("normal_map_enabled", normal_map_enabled);
        adapter("normal_map", m_normal_map_name);
        if (loading)
            normal_map(m_normal_map_name);

        adapter("emissive_map_enabled", emissive_map_enabled);
        adapter("emissive_color", emissive_color);
        adapter("emissive_map", m_emissive_map_name);
        if (loading)
            emissive_map(m_emissive_map_name);
    }

    BIRDY3D_REGISTER_TYPE_DEF(Material);

}