#include "render/Material.hpp"

#include "render/Shader.hpp"

namespace Birdy3d::render {

    void Material::diffuse_map(core::ResourceIdentifier const& id)
    {
        m_diffuse_map = id;
    }

    void Material::specular_map(core::ResourceIdentifier const& id)
    {
        m_specular_map = id;
    }

    void Material::normal_map(core::ResourceIdentifier const& id)
    {
        m_normal_map = id;
    }

    void Material::emissive_map(core::ResourceIdentifier const& id)
    {
        m_emissive_map = id;
    }

    void Material::use(Shader const& shader) const
    {
        if (m_cached_shader_id != shader.id()) {
            m_cached_shader_id = shader.id();
            m_cached_shader_uniform_locations.diffuse_map_enabled = shader.get_uniform_location("material.diffuse_map_enabled");
            m_cached_shader_uniform_locations.diffuse_color = shader.get_uniform_location("material.diffuse_color");
            m_cached_shader_uniform_locations.diffuse_map = shader.get_uniform_location("material.diffuse_map");

            m_cached_shader_uniform_locations.specular_map_enabled = shader.get_uniform_location("material.specular_map_enabled");
            m_cached_shader_uniform_locations.specular_value = shader.get_uniform_location("material.specular_value");
            m_cached_shader_uniform_locations.specular_map = shader.get_uniform_location("material.specular_map");

            m_cached_shader_uniform_locations.normal_map_enabled = shader.get_uniform_location("material.normal_map_enabled");
            m_cached_shader_uniform_locations.normal_map = shader.get_uniform_location("material.normal_map");

            m_cached_shader_uniform_locations.emissive_map_enabled = shader.get_uniform_location("material.emissive_map_enabled");
            m_cached_shader_uniform_locations.emissive_color = shader.get_uniform_location("material.emissive_color");
            m_cached_shader_uniform_locations.emissive_map = shader.get_uniform_location("material.emissive_map");
        }

        shader.set_uniform(m_cached_shader_uniform_locations.diffuse_map_enabled, diffuse_map_enabled);
        shader.set_uniform(m_cached_shader_uniform_locations.diffuse_color, diffuse_color.value);
        m_diffuse_map->bind(0);
        shader.set_uniform(m_cached_shader_uniform_locations.diffuse_map, 0);

        shader.set_uniform(m_cached_shader_uniform_locations.specular_map_enabled, specular_map_enabled);
        shader.set_uniform(m_cached_shader_uniform_locations.specular_value, specular_value);
        m_specular_map->bind(1);
        shader.set_uniform(m_cached_shader_uniform_locations.specular_map, 1);

        shader.set_uniform(m_cached_shader_uniform_locations.normal_map_enabled, normal_map_enabled);
        m_normal_map->bind(2);
        shader.set_uniform(m_cached_shader_uniform_locations.normal_map, 2);

        shader.set_uniform(m_cached_shader_uniform_locations.emissive_map_enabled, emissive_map_enabled);
        shader.set_uniform(m_cached_shader_uniform_locations.emissive_color, emissive_color.value);
        m_emissive_map->bind(3);
        shader.set_uniform(m_cached_shader_uniform_locations.emissive_map, 3);
    }

    bool Material::transparent() const
    {
        if (diffuse_map_enabled)
            return m_diffuse_map->transparent();
        else
            return diffuse_color.value.a < 1;
    }

    void Material::serialize(serializer::Adapter& adapter)
    {
        adapter("diffuse_map_enabled", diffuse_map_enabled);
        adapter("diffuse_color", diffuse_color);
        adapter("diffuse_map", m_diffuse_map);

        adapter("specular_map_enabled", specular_map_enabled);
        adapter("specular_value", specular_value);
        adapter("specular_map", m_specular_map);

        adapter("normal_map_enabled", normal_map_enabled);
        adapter("normal_map", m_normal_map);

        adapter("emissive_map_enabled", emissive_map_enabled);
        adapter("emissive_color", emissive_color);
        adapter("emissive_map", m_emissive_map);
    }

    BIRDY3D_REGISTER_TYPE_DEF(Material);

}
