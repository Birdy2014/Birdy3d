#include "render/Material.hpp"

#include "render/Shader.hpp"

namespace Birdy3d {

    void Material::use(const Shader& shader) const {
        shader.setBool("material.diffuse_map_enabled", diffuse_map_enabled);
        shader.setVec4("material.diffuse_color", diffuse_color);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, diffuse_map->id);
        shader.setInt("material.diffuse_map", 0);

        shader.setBool("material.specular_map_enabled", specular_map_enabled);
        shader.setFloat("material.specular_value", specular_value);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, specular_map->id);
        shader.setInt("material.specular_map", 1);

        shader.setBool("material.normal_map_enabled", normal_map_enabled);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, normal_map->id);
        shader.setInt("material.normal_map", 2);

        shader.setBool("material.emissive_map_enabled", emissive_map_enabled);
        shader.setVec4("material.emissive_color", emissive_color);
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, emissive_map->id);
        shader.setInt("material.emissive_map", 3);
    }

    bool Material::transparent() const {
        if (diffuse_map_enabled)
            return diffuse_map->transparent();
        else
            return diffuse_color.value.a < 1;
    }

}
