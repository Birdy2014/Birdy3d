#pragma once

#include "core/RessourceManager.hpp"
#include "render/Texture.hpp"
#include <glm/glm.hpp>
#include <memory>

namespace Birdy3d {

    class Material {
    public:
        bool diffuse_map_enabled = false;
        Color diffuse_color = Color::WHITE;
        std::shared_ptr<Texture> diffuse_map = RessourceManager::getColorTexture(Color::WHITE);

        bool specular_map_enabled = false;
        float specular_value = 32.0f / 255.0f;
        std::shared_ptr<Texture> specular_map = RessourceManager::getColorTexture(glm::vec4(32.0f / 255.0f));

        bool normal_map_enabled = false;
        std::shared_ptr<Texture> normal_map = RessourceManager::getColorTexture(Color::WHITE);

        bool emissive_map_enabled = false;
        Color emissive_color = Color::BLACK;
        std::shared_ptr<Texture> emissive_map = RessourceManager::getColorTexture(Color::BLACK);

        bool transparent() const {
            if (diffuse_map_enabled)
                return diffuse_map->transparent();
            else
                return diffuse_color.value.a < 1;
        }
    };

}
