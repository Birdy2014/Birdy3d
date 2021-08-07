#pragma once

#include "core/Base.hpp"
#include "core/Logger.hpp"
#include "render/Color.hpp"

namespace Birdy3d {

    class Shader;
    class TextRenderer;
    class Model;
    class Texture;

    class RessourceManager {
    public:
        enum class RessourceType {
            SHADER,
            TEXTURE,
            MODEL,
            FONT
        };

        static std::shared_ptr<Shader> getShader(const std::string& name);
        static std::shared_ptr<TextRenderer> getTextRenderer(const std::string& name);
        static std::shared_ptr<Model> getModel(const std::string& name);
        static std::shared_ptr<Texture> getTexture(const std::string& name);
        static std::shared_ptr<Texture> getColorTexture(const Color& color);
        static std::string getRessourcePath(std::string name, RessourceType type);
        static std::string readFile(const std::string& path, bool convertEOL = true);

    private:
        static std::unordered_map<std::string, std::shared_ptr<Shader>> shaders;
        static std::unordered_map<std::string, std::shared_ptr<TextRenderer>> textRenderers;
        static std::unordered_map<std::string, std::shared_ptr<Model>> models;
        static std::unordered_map<std::string, std::shared_ptr<Texture>> textures;
        static std::map<Color, std::shared_ptr<Texture>> m_color_textures;

        static std::string getExecutableDir();
    };

}
