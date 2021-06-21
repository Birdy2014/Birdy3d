#pragma once

#include "core/RessourceManager.hpp"
#include "render/Texture.hpp"
#include <glm/glm.hpp>

namespace Birdy3d {

    class Material {
    public:
        bool m_has_normal_map = false;

        Texture* diffuse_map() const {
            return m_diffuse_map;
        }

        void diffuse_color(Color value) {
            m_diffuse_map = RessourceManager::getColorTexture(value);
        }

        void diffuse_map(const std::string& name) {
            m_diffuse_map = RessourceManager::getTexture(name);
        }

        Texture* specular_map() const {
            return m_specular_map;
        }

        void specular(float value) {
            m_specular_map = RessourceManager::getColorTexture(glm::vec4(value, 0, 0, 0));
        }

        void specular_map(const std::string& name) {
            m_specular_map = RessourceManager::getTexture(name);
        }

        Texture* normal_map() const {
            return m_normal_map;
        }

        void normal_map(const std::string& name) {
            m_normal_map = RessourceManager::getTexture(name);
        }

        Texture* emissive_map() const {
            return m_emissive_map;
        }

        void emissive_color(Color value) {
            m_emissive_map = RessourceManager::getColorTexture(value);
        }

        void emissive_map(const std::string& name) {
            m_emissive_map = RessourceManager::getTexture(name);
        }

        bool transparent() const {
            return m_diffuse_map->transparent();
        }

    private:
        Texture* m_diffuse_map = RessourceManager::getColorTexture(Color::WHITE);
        Texture* m_specular_map = RessourceManager::getColorTexture(glm::vec4(32.0f / 255.0f));
        Texture* m_normal_map = RessourceManager::getColorTexture(Color::WHITE);
        Texture* m_emissive_map = RessourceManager::getColorTexture(Color::BLACK);
    };

}
