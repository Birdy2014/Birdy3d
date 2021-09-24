#pragma once

#include "core/Base.hpp"
#include "core/Logger.hpp"
#include "render/Color.hpp"

namespace Birdy3d {

    class Shader;
    class Theme;
    class Model;
    class Texture;

    class RessourceManager {
    public:
        enum class RessourceType {
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
        static std::string get_ressource_path(std::string name, RessourceType type);
        static std::string read_file(const std::string& path, bool convertEOL = true);
        static void init();

    private:
        static std::unordered_map<std::string, std::shared_ptr<Shader>> m_shaders;
        static std::unordered_map<std::string, std::shared_ptr<Theme>> m_themes;
        static std::unordered_map<std::string, std::shared_ptr<Model>> m_models;
        static std::unordered_map<std::string, std::shared_ptr<Texture>> m_textures;

        static std::string get_executable_dir();
    };

}
