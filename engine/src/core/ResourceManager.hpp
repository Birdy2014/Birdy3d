#pragma once

#include "core/Base.hpp"
#include "core/Logger.hpp"
#include "utils/Color.hpp"

namespace Birdy3d {

    class Shader;
    class Theme;
    class Model;
    class Texture;

    class ResourceManager {
    public:
        enum class ResourceType {
            SHADER,
            TEXTURE,
            THEME,
            MODEL,
            FONT
        };

        static std::shared_ptr<Shader> get_shader(const std::string& name);
        static std::shared_ptr<Theme> get_theme(const std::string& name);
        static std::shared_ptr<Model> get_model(const std::string& name);
        static std::shared_ptr<Texture> get_texture(const std::string& name);
        static std::shared_ptr<Texture> get_color_texture(const Color&);

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
        static std::string read_file(const std::string& path, bool convert_eol = true);

        static void init();

    private:
        static std::unordered_map<std::string, std::shared_ptr<Shader>> m_shaders;
        static std::unordered_map<std::string, std::shared_ptr<Theme>> m_themes;
        static std::unordered_map<std::string, std::shared_ptr<Model>> m_models;
        static std::unordered_map<std::string, std::shared_ptr<Texture>> m_textures;

        static std::string search_for_file(std::string directory, std::string filename);
        static std::string get_executable_dir();
    };

}
