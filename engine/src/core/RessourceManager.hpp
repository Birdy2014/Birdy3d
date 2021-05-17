#pragma once

#include <string>
#include <unordered_map>

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

        static Shader* getShader(const std::string& name);
        static TextRenderer* getTextRenderer(const std::string& name);
        static Model* getModel(const std::string& name);
        static Texture* getTexture(const std::string& name, const std::string& type);
        static std::string getRessourcePath(std::string name, RessourceType type);
        static std::string readFile(const std::string& path, bool convertEOL = true);

    private:
        static std::unordered_map<std::string, Shader*> shaders;
        static std::unordered_map<std::string, TextRenderer*> textRenderers;
        static std::unordered_map<std::string, Model*> models;
        static std::unordered_map<std::string, Texture*> textures;

        static std::string getExecutableDir();
    };

}
