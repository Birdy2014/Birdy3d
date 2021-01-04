#ifndef BIRDY3D_RESSOURCEMANAGER_HPP
#define BIRDY3D_RESSOURCEMANAGER_HPP

#include "render/Shader.hpp"
#include <string>
#include <unordered_map>

class RessourceManager {
public:
    enum class RessourceType {
        SHADER,
        TEXTURE,
        MODEL,
        FONT
    };

    static Shader *getShader(const std::string &name);
    static std::string getFontPath(std::string name);
    static std::string getRessourcePath(std::string &name, RessourceType type);
    static std::string readFile(const std::string &path, bool convertEOL = true);

private:
    static std::unordered_map<std::string, Shader*> shaders;

    static Shader *loadShader(std::string name);
    static std::string getExecutableDir();
};

#endif
