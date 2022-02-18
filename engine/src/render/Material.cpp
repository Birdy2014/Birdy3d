#include "render/Material.hpp"

#include "render/Shader.hpp"

namespace Birdy3d::render {

    void Material::diffuse_map(const core::ResourceIdentifier& id) {
        m_diffuse_map = id;
    }

    void Material::specular_map(const core::ResourceIdentifier& id) {
        m_specular_map = id;
    }

    void Material::normal_map(const core::ResourceIdentifier& id) {
        m_normal_map = id;
    }

    void Material::emissive_map(const core::ResourceIdentifier& id) {
        m_emissive_map = id;
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
