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
        static std::shared_ptr<Texture> getColorTexture(const Color&);
        static std::string getRessourcePath(std::string name, RessourceType type);
        static std::string readFile(const std::string& path, bool convertEOL = true);

    private:
        static std::unordered_map<std::string, std::shared_ptr<Shader>> m_shaders;
        static std::unordered_map<std::string, std::shared_ptr<TextRenderer>> m_text_renderers;
        static std::unordered_map<std::string, std::shared_ptr<Model>> m_models;
        static std::unordered_map<std::string, std::shared_ptr<Texture>> m_textures;

        static std::string getExecutableDir();
    };

}
