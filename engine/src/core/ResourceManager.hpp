#pragma once

#include "core/Base.hpp"
#include "core/ResourceHandle.hpp"
#include "render/Forward.hpp"
#include "ui/Forward.hpp"

namespace Birdy3d::core {

    class ResourceManager {
    public:
        static ResourceHandle<render::Shader> get_shader(ResourceIdentifier const& id);
        static ResourceHandle<ui::Theme> get_theme(ResourceIdentifier const& id);
        static ResourceHandle<render::Model> get_model(ResourceIdentifier const& id);
        static ResourceHandle<render::Texture> get_texture(ResourceIdentifier const& id);
        static ResourceHandle<physics::Collider> get_collider(ResourceIdentifier const& id);

        /**
         * @brief Finds the path of a Resource.
         * @param name resource name
         * @param type resource type
         * @returns path or an empty string if the resource couldn't be found
         */
        static std::string get_resource_path(std::string name, ResourceType type);

        /**
         * @brief Reads the contents of a file.
         * @param path file path
         * @param convert_eol convert Windows file endings (CRLF) to UNIX file endings (LF)
         * @returns file content or an empty string if the file was not found
         */
        static std::string read_file(std::string const& path, bool convert_eol = true);

        static std::string get_resource_dir();

    private:
        friend class ResourceHandle<render::Shader>;
        friend class ResourceHandle<ui::Theme>;
        friend class ResourceHandle<render::Model>;
        friend class ResourceHandle<render::Texture>;
        friend class ResourceHandle<physics::Collider>;

        static std::unordered_map<std::string, std::size_t> m_shader_indices;
        static std::unordered_map<std::string, std::size_t> m_theme_indices;
        static std::unordered_map<std::string, std::size_t> m_model_indices;
        static std::unordered_map<std::string, std::size_t> m_texture_indices;
        static std::unordered_map<std::string, std::size_t> m_collider_indices;

        static std::vector<std::unique_ptr<render::Shader>> m_shaders;
        static std::vector<std::unique_ptr<ui::Theme>> m_themes;
        static std::vector<std::unique_ptr<render::Model>> m_models;
        static std::vector<std::unique_ptr<render::Texture>> m_textures;
        static std::vector<std::unique_ptr<physics::Collider>> m_colliders;

        static std::optional<std::size_t> load_shader_ptr(ResourceIdentifier const&);
        static std::optional<std::size_t> load_theme_ptr(ResourceIdentifier const&);
        static std::optional<std::size_t> load_model_ptr(ResourceIdentifier const&);
        static std::optional<std::size_t> load_texture_ptr(ResourceIdentifier const&);
        static std::optional<std::size_t> load_collider_ptr(ResourceIdentifier const&);

        static render::Shader const* get_shader_ptr(std::size_t const&);
        static ui::Theme const* get_theme_ptr(std::size_t const&);
        static render::Model const* get_model_ptr(std::size_t const&);
        static render::Texture const* get_texture_ptr(std::size_t const&);
        static physics::Collider const* get_collider_ptr(std::size_t const&);

        static std::string get_executable_dir();
    };

}
