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

namespace Birdy3d::core {

    template <>
    bool ResourceHandle<render::Shader>::load(std::string new_name) {
        auto val = ResourceManager::get_shader_ptr(new_name);
        if (!val)
            return false;
        m_ptr = val;
        return true;
    }

    template <>
    bool ResourceHandle<ui::Theme>::load(std::string new_name) {
        auto val = ResourceManager::get_theme_ptr(new_name);
        if (!val)
            return false;
        m_ptr = val;
        return true;
    }

    template <>
    bool ResourceHandle<render::Model>::load(std::string new_name) {
        auto val = ResourceManager::get_model_ptr(new_name);
        if (!val)
            return false;
        m_ptr = val;
        return true;
    }

    template <>
    bool ResourceHandle<render::Texture>::load(std::string new_name) {
        auto val = ResourceManager::get_texture_ptr(new_name);
        if (!val)
            return false;
        m_ptr = val;
        return true;
    }

    ResourceManager::ResourceIdentifier::ResourceIdentifier(std::string full_name) {
        std::vector<std::string> parts;
        std::size_t last_pos = 0;
        std::size_t current_pos = full_name.find_first_of(':');
        while (last_pos != std::string::npos) {
            parts.push_back(full_name.substr(last_pos, current_pos - last_pos));
            last_pos = current_pos == std::string::npos ? current_pos : current_pos + 1;
            current_pos = full_name.find_first_of(':', current_pos + 1);
        }

        std::size_t args_start = std::string::npos;
        if (parts.size() >= 3 && parts[1] == "") {
            source = parts[0];
            name = parts[2];
            args_start = 3;
        } else if (parts.size() >= 1) {
            name = parts[0];
            args_start = 1;
        } else {
            return;
        }

        if (parts.size() >= args_start) {
            for (auto it = parts.begin() + args_start; it != parts.end(); it++) {
                if (it->empty())
                    continue;
                args.push_back(*it);
            }
        }
    }

    std::unordered_map<std::string, std::shared_ptr<render::Shader>> ResourceManager::m_shaders;
    std::unordered_map<std::string, std::shared_ptr<ui::Theme>> ResourceManager::m_themes;
    std::unordered_map<std::string, std::shared_ptr<render::Model>> ResourceManager::m_models;
    std::unordered_map<std::string, std::shared_ptr<render::Texture>> ResourceManager::m_textures;

    void ResourceManager::init() {
        std::string base_path = get_executable_dir() + "../resources/";
        std::filesystem::current_path(base_path);
    }

    ResourceHandle<render::Shader> ResourceManager::get_shader(const std::string& name) {
        return ResourceHandle<render::Shader>(name);
    }

    ResourceHandle<ui::Theme> ResourceManager::get_theme(const std::string& name) {
        return ResourceHandle<ui::Theme>(name);
    }

    ResourceHandle<render::Model> ResourceManager::get_model(const std::string& name) {
        return ResourceHandle<render::Model>(name);
    }

    ResourceHandle<render::Texture> ResourceManager::get_texture(const std::string& name) {
        return ResourceHandle<render::Texture>(name);
    }

    std::shared_ptr<render::Shader> ResourceManager::get_shader_ptr(const std::string& name) {
        std::shared_ptr<render::Shader> shader = m_shaders[name];
        if (!shader) {
            ResourceIdentifier id { name };
            if (id.source == "file" || id.source == "") {
                shader = std::make_shared<render::Shader>(id.name);
            } else {
                Logger::error("invalid shader source");
            }
            m_shaders[name] = shader;
        }
        return shader;
    }

    std::shared_ptr<ui::Theme> ResourceManager::get_theme_ptr(const std::string& name) {
        std::shared_ptr<ui::Theme> theme = m_themes[name];
        if (!theme) {
            ResourceIdentifier id { name };
            if (id.source == "file" || id.source == "") {
                std::string path = get_resource_path(id.name, ResourceType::THEME);
                if (path.empty())
                    return nullptr;
                std::string file_content = ResourceManager::read_file(path);
                if (file_content.empty())
                    return nullptr;
                try {
                    theme = std::make_shared<ui::Theme>(file_content);
                } catch (const std::exception& e) {
                    return nullptr;
                }
            } else {
                Logger::error("invalid shader source");
            }
            m_themes[name] = theme;
        }
        return theme;
    }

    std::shared_ptr<render::Model> ResourceManager::get_model_ptr(const std::string& name) {
        std::shared_ptr<render::Model> model = m_models[name];
        if (!model) {
            ResourceIdentifier id { name };
            if (id.source == "file" || id.source == "") {
                std::string path = get_resource_path(id.name, ResourceType::MODEL);
                if (path.empty())
                    return nullptr;
                model = std::make_shared<render::Model>(path);
            } else if (id.source == "primitive") {
                if (id.name == "plane")
                    model = utils::PrimitiveGenerator::generate_plane();
                else if (id.name == "cube")
                    model = utils::PrimitiveGenerator::generate_cube();
                else if (id.name == "uv_sphere" && id.args.size() == 1)
                    model = utils::PrimitiveGenerator::generate_uv_sphere(std::stoi(id.args[0]));
                else if (id.name == "ico_sphere" && id.args.size() == 1)
                    model = utils::PrimitiveGenerator::generate_ico_sphere(std::stoi(id.args[0]));
                else
                    Logger::error("invalid primitive type");
            } else {
                Logger::error("invalid model source");
            }
            m_models[name] = model;
        }
        return model;
    }

    std::shared_ptr<render::Texture> ResourceManager::get_texture_ptr(const std::string& name) {
        std::shared_ptr<render::Texture> texture = m_textures[name];
        if (!texture) {
            ResourceIdentifier id { name };
            if (id.source == "file" || id.source == "") {
                std::string path = get_resource_path(id.name, ResourceType::TEXTURE);
                if (path.empty())
                    return nullptr;
                texture = std::make_shared<render::Texture>(path);
            } else if (id.source == "color") {
                utils::Color color = id.name;
                texture = std::make_shared<render::Texture>(color);
            } else {
                Logger::error("invalid texture source");
            }
            m_textures[name] = texture;
        }
        return texture;
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

        if (std::filesystem::is_regular_file(name + extension))
            return name + extension;
        if (std::filesystem::is_regular_file("/" + name + extension))
            return "/" + name + extension;
        if (std::filesystem::is_regular_file(subdir + name + extension))
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
