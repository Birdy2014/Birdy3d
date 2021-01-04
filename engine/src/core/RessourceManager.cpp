#include "core/RessourceManager.hpp"

#include <filesystem>
#include <string>
#include <algorithm>

#ifdef __linux__
#include <limits.h>
#include <unistd.h>
#elif _WIN32
#include <windows.h>
#endif

std::unordered_map<std::string, Shader*> RessourceManager::shaders;

Shader *RessourceManager::getShader(const std::string &name) {
    Shader *shader = shaders[name];
    if (!shader) {
        shader = loadShader(name);
    }
    return shader;
}

Shader *RessourceManager::loadShader(std::string name) {
    std::string path = getRessourcePath(name, RessourceType::SHADER);
    Shader *s = new Shader(readFile(path));
    shaders[name] = s;
    return s;
}

std::string RessourceManager::getRessourcePath(std::string &name, RessourceType type) {
    if (name.size() == 0) {
        std::cout << "ERROR: invalid ressource name" << std::endl;
        return nullptr;
    }

    std::replace(name.begin(), name.end(), '\\', '/');

    if (name.front() == '/')
        name.erase(0, 1);
    
    std::string extension;
    size_t dotPos = name.find_last_of('.');
    if (dotPos != std::string::npos) {
        extension = name.substr(dotPos);
        name = name.substr(0, dotPos);
    }

    std::string base_path = getExecutableDir() + "ressources/";
    std::string default_dir = getExecutableDir();
    std::string subdir;
    switch (type) {
        case RessourceType::SHADER:
            subdir = "shaders/";
            default_dir += "shaders/";
            if (extension.size() == 0)
                extension = ".glsl";
            break;
        case RessourceType::TEXTURE:
            subdir = "textures/";
            break;
        case RessourceType::MODEL:
            subdir = "models/";
            break;
        case RessourceType::FONT:
            subdir = "fonts/";
            if (extension.size() == 0)
                extension = ".ttf";
            break;
        default:
            return nullptr;
    }

    if (std::filesystem::exists(base_path + subdir + name + extension))
        return base_path + subdir + name + extension;
    if (std::filesystem::exists(base_path + name + extension))
        return base_path + name + extension;
    if (std::filesystem::exists(default_dir + name + extension))
        return default_dir + name + extension;

    std::cout << "ERROR: can't find ressource " << name << std::endl;
    return "";
}

std::string RessourceManager::getExecutableDir() {
#ifdef __linux__
    char result[PATH_MAX];
    ssize_t count = readlink("/proc/self/exe", result, PATH_MAX);
    std::string exec = std::string(result, (count > 0) ? count : 0);
#elif __WIN32
    char result[MAX_PATH];
    std::string exec = std::string(result, GetModuleFileName(NULL, result, MAX_PATH));
#endif
    return exec.substr(0, exec.find_last_of("/\\") + 1);
}

std::string RessourceManager::readFile(const std::string &path, bool convertEOL) {
    std::ifstream fileStream;
    std::string content;
    try {
        fileStream.open(path);
        content.assign(std::istreambuf_iterator<char>(fileStream), std::istreambuf_iterator<char>());
        if (convertEOL)
            content.erase(std::remove(content.begin(), content.end(), '\r'), content.end());
    } catch (std::ifstream::failure &e) {
        std::cout << "ERROR: Failed to read file " << path << std::endl;
    }
    return content;
}
