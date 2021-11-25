#include "core/ResourceManager.hpp"

#include "core/Logger.hpp"
#include "render/Model.hpp"
#include "render/Shader.hpp"
#include "render/Texture.hpp"
#include "ui/Theme.hpp"
#include "utils/PrimitiveGenerator.hpp"
#include <algorithm>
#include <filesystem>
#include <fstream>
#include <string>

#if defined(BIRDY3D_PLATFORM_LINUX)
    #include <limits.h>
    #include <unistd.h>
#elif defined(BIRDY3D_PLATFORM_WINDOWS)
    #include <windows.h>
#endif

namespace Birdy3d {

    std::unordered_map<std::string, std::shared_ptr<Shader>> ResourceManager::m_shaders;
    std::unordered_map<std::string, std::shared_ptr<Theme>> ResourceManager::m_themes;
    std::unordered_map<std::string, std::shared_ptr<Model>> ResourceManager::m_models;
    std::unordered_map<std::string, std::shared_ptr<Texture>> ResourceManager::m_textures;

    void ResourceManager::init() {
        std::string base_path = get_executable_dir() + "../resources/";
        std::filesystem::current_path(base_path);
    }

    std::shared_ptr<Shader> ResourceManager::get_shader(const std::string& name) {
        std::shared_ptr<Shader> shader = m_shaders[name];
        if (!shader) {
            shader = std::make_shared<Shader>(name);
            m_shaders[name] = shader;
        }
        return shader;
    }

    std::shared_ptr<Theme> ResourceManager::get_theme(const std::string& name) {
        std::shared_ptr<Theme> theme = m_themes[name];
        if (!theme) {
            std::string path = get_resource_path(name, ResourceType::THEME);
            if (path.empty())
                return nullptr;
            std::string file_content = ResourceManager::read_file(path);
            if (file_content.empty())
                return nullptr;
            try {
                theme = std::make_shared<Theme>(file_content);
            } catch (std::exception e) {
                return nullptr;
            }
            m_themes[name] = theme;
        }
        return theme;
    }

    std::shared_ptr<Model> ResourceManager::get_model(const std::string& name) {
        std::shared_ptr<Model> model = m_models[name];
        if (!model) {
            // TODO: generalize for all resources
            std::string prefix_primitive = "primitive::";
            if (name.starts_with(prefix_primitive)) {
                std::string primitive_type, arg;
                std::string rest = name.substr(prefix_primitive.length());
                size_t arg_separator_pos = rest.find(':');
                if (arg_separator_pos == std::string::npos) {
                    primitive_type = rest;
                } else {
                    primitive_type = rest.substr(0, arg_separator_pos);
                    arg = rest.substr(arg_separator_pos + 1);
                }
                if (primitive_type == "plane")
                    model = PrimitiveGenerator::generate_plane();
                else if (primitive_type == "cube")
                    model = PrimitiveGenerator::generate_cube();
                else if (primitive_type == "uv_sphere")
                    model = PrimitiveGenerator::generate_uv_sphere(std::stoi(arg));
                else if (primitive_type == "ico_sphere")
                    model = PrimitiveGenerator::generate_ico_sphere(std::stoi(arg));
                else
                    Logger::error("invalid primitive type");
            } else {
                std::string path = get_resource_path(name, ResourceType::MODEL);
                model = std::make_shared<Model>(path);
            }
            m_models[name] = model;
        }
        return model;
    }

    std::shared_ptr<Texture> ResourceManager::get_texture(const std::string& name) {
        std::shared_ptr<Texture> texture = m_textures[name];
        if (!texture) {
            std::string prefix_color = "color::";
            if (name.starts_with(prefix_color)) {
                Color color = name.substr(prefix_color.length());
                texture = std::make_shared<Texture>(color);
            } else {
                std::string path = get_resource_path(name, ResourceType::TEXTURE);
                texture = std::make_shared<Texture>(path);
            }
            m_textures[name] = texture;
        }
        return texture;
    }

    std::shared_ptr<Texture> ResourceManager::get_color_texture(const Color& color) {
        return get_texture("color::" + color.to_string());
    }

    std::string ResourceManager::get_resource_path(std::string name, ResourceType type) {
        if (name.size() == 0) {
            Logger::error("invalid resource name");
            return {};
        }

        std::replace(name.begin(), name.end(), '\\', '/');

        if (name.front() == '/')
            name.erase(0, 1);

        std::string extension;
        std::size_t dot_pos = name.find_last_of('.');
        if (dot_pos != std::string::npos) {
            extension = name.substr(dot_pos);
            name = name.substr(0, dot_pos);
        }

        std::string default_dir;
        std::string subdir;
        switch (type) {
        case ResourceType::SHADER:
            default_dir = "../shaders/";
            subdir = "shaders/";
            if (extension.size() == 0)
                extension = ".glsl";
            break;
        case ResourceType::TEXTURE:
            subdir = "textures/";
            break;
        case ResourceType::THEME:
            subdir = "themes/";
            break;
        case ResourceType::MODEL:
            subdir = "models/";
            break;
        case ResourceType::FONT:
            subdir = "fonts/";
#if defined(BIRDY3D_PLATFORM_LINUX)
            default_dir = "/usr/share/fonts/";
#elif defined(BIRDY3D_PLATFORM_WINDOWS)
            default_dir = "C:/Windows/Fonts/";
#else
            default_dir = "";
#endif
            if (extension.size() == 0)
                extension = ".ttf";
            break;
        default:
            return {};
        }

        if (std::filesystem::exists(name + extension))
            return name + extension;
        if (std::filesystem::exists("/" + name + extension))
            return "/" + name + extension;
        if (std::filesystem::exists(subdir + name + extension))
            return subdir + name + extension;

        std::string path_fragment;
        std::size_t last_slash = name.find_last_of('/');
        if (last_slash != std::string::npos) {
            path_fragment = name.substr(0, last_slash + 1);
            name = name.substr(last_slash + 1);
        }

        std::string output_path;
        output_path = search_for_file(default_dir + path_fragment, name + extension);
        if (!output_path.empty())
            return output_path;

        Logger::error("can't find resource ", name, extension);
        return {};
    }

    std::string ResourceManager::get_executable_dir() {
#if defined(BIRDY3D_PLATFORM_LINUX)
        char result[PATH_MAX];
        ssize_t count = readlink("/proc/self/exe", result, PATH_MAX);
        std::string exec = std::string(result, (count > 0) ? count : 0);
#elif defined(BIRDY3D_PLATFORM_WINDOWS)
        char result[MAX_PATH];
        std::string exec = std::string(result, GetModuleFileName(NULL, result, MAX_PATH));
#endif
        return exec.substr(0, exec.find_last_of("/\\") + 1);
    }

    std::string ResourceManager::read_file(const std::string& path, bool convert_eol) {
        std::ifstream file_stream;
        std::string content;
        try {
            file_stream.open(path);
            content.assign(std::istreambuf_iterator<char>(file_stream), std::istreambuf_iterator<char>());
            if (convert_eol)
                content.erase(std::remove(content.begin(), content.end(), '\r'), content.end());
        } catch (std::ifstream::failure& e) {
            Logger::error("Failed to read file ", path);
        }
        return content;
    }

    std::string ResourceManager::search_for_file(std::string directory, std::string filename) {
        if (!std::filesystem::is_directory(directory))
            return {};
        for (const auto& entry : std::filesystem::directory_iterator(directory)) {
            if (entry.is_regular_file() && entry.path().filename() == filename)
                return entry.path().string();
            if (entry.is_directory()) {
                std::string found = search_for_file(entry.path().string(), filename);
                if (!found.empty())
                    return found;
            }
        }
        return {};
    }

}
