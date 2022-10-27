#pragma once

#include "core/Base.hpp"
#include "core/ResourceHandle.hpp"
#include "render/Forward.hpp"
#include "ui/Forward.hpp"

namespace Birdy3d::core {

    class ResourceManager {
    public:
        static ResourceHandle<render::Shader> get_shader(std::string const& name);
        static ResourceHandle<ui::Theme> get_theme(std::string const& name);
        static ResourceHandle<render::Model> get_model(std::string const& name);
        static ResourceHandle<render::Texture> get_texture(std::string const& name);

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

        static std::unordered_map<std::string, std::shared_ptr<render::Shader>> m_shaders;
        static std::unordered_map<std::string, std::shared_ptr<ui::Theme>> m_themes;
        static std::unordered_map<std::string, std::shared_ptr<render::Model>> m_models;
        static std::unordered_map<std::string, std::shared_ptr<render::Texture>> m_textures;

        static std::shared_ptr<render::Shader> get_shader_ptr(ResourceIdentifier const&);
        static std::shared_ptr<ui::Theme> get_theme_ptr(ResourceIdentifier const&);
        static std::shared_ptr<render::Model> get_model_ptr(ResourceIdentifier const&);
        static std::shared_ptr<render::Texture> get_texture_ptr(ResourceIdentifier const&);

        static std::string get_executable_dir();
    };

}
