#include "core/RessourceManager.hpp"

#include "core/Logger.hpp"
#include "render/Model.hpp"
#include "render/Shader.hpp"
#include "render/Texture.hpp"
#include "ui/TextRenderer.hpp"
#include <algorithm>
#include <filesystem>
#include <fstream>
#include <string>

#ifdef __linux__
#include <limits.h>
#include <unistd.h>
#elif _WIN32
#include <windows.h>
#endif

namespace Birdy3d {

    std::unordered_map<std::string, Shader*> RessourceManager::shaders;
    std::unordered_map<std::string, TextRenderer*> RessourceManager::textRenderers;
    std::unordered_map<std::string, Model*> RessourceManager::models;
    std::unordered_map<std::string, Texture*> RessourceManager::textures;

    Shader* RessourceManager::getShader(const std::string& name) {
        Shader* shader = shaders[name];
        if (!shader) {
            std::string path = getRessourcePath(name, RessourceType::SHADER);
            shader = new Shader(readFile(path), name);
            shaders[name] = shader;
        }
        return shader;
    }

    TextRenderer* RessourceManager::getTextRenderer(const std::string& name) {
        TextRenderer* renderer = textRenderers[name];
        if (!renderer) {
            std::string path = getRessourcePath(name, RessourceType::FONT);
            renderer = new TextRenderer(path, 30);
            textRenderers[name] = renderer;
        }
        return renderer;
    }

    Model* RessourceManager::getModel(const std::string& name) {
        Model* model = models[name];
        if (!model) {
            std::string path = getRessourcePath(name, RessourceType::MODEL);
            model = new Model(path);
            models[name] = model;
        }
        return model;
    }

    Texture* RessourceManager::getTexture(const std::string& name, const std::string& type) {
        Texture* texture = textures[name];
        if (!texture) {
            std::string path = getRessourcePath(name, RessourceType::TEXTURE);
            texture = new Texture(path, type, name);
            textures[name] = texture;
        }
        return texture;
    }

    std::string RessourceManager::getRessourcePath(std::string name, RessourceType type) {
        if (name.size() == 0) {
            Logger::error("invalid ressource name");
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

        std::string base_path = getExecutableDir() + "../ressources/";
        std::string default_dir = getExecutableDir() + "../";
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
            default_dir = "/usr/share/fonts/";
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
        if (std::filesystem::exists("/" + name + extension))
            return "/" + name + extension;

        Logger::error("can't find ressource " + name + extension);
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

    std::string RessourceManager::readFile(const std::string& path, bool convertEOL) {
        std::ifstream fileStream;
        std::string content;
        try {
            fileStream.open(path);
            content.assign(std::istreambuf_iterator<char>(fileStream), std::istreambuf_iterator<char>());
            if (convertEOL)
                content.erase(std::remove(content.begin(), content.end(), '\r'), content.end());
        } catch (std::ifstream::failure& e) {
            Logger::error("Failed to read file " + path);
        }
        return content;
    }

}
