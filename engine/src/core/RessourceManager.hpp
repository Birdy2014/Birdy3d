#pragma once

#include <string>
#include <unordered_map>

namespace Birdy3d {

    class Shader;
    class TextRenderer;
    class Model;

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
        static Model* getModel(const std::string& options);
        static std::string getRessourcePath(std::string name, RessourceType type);
        static std::string readFile(const std::string& path, bool convertEOL = true);

    private:
        static std::unordered_map<std::string, Shader*> shaders;
        static std::unordered_map<std::string, TextRenderer*> textRenderers;
        static std::unordered_map<std::string, Model*> models;

        static Shader* loadShader(std::string name);
        static std::string getExecutableDir();
    };

}
