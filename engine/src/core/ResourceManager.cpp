#include "core/ResourceManager.hpp"

#include "core/Logger.hpp"
#include "physics/Collider.hpp"
#include "physics/CollisionSphere.hpp"
#include "physics/ConvexMeshGenerators.hpp"
#include "render/Model.hpp"
#include "render/Shader.hpp"
#include "render/Texture.hpp"
#include "ui/Theme.hpp"
#include "utils/PrimitiveGenerator.hpp"
#include <algorithm>
#include <filesystem>
#include <fstream>
#include <numeric>
#include <string>

#if defined(BIRDY3D_PLATFORM_LINUX)
    #include <limits.h>
    #include <unistd.h>
#elif defined(BIRDY3D_PLATFORM_WINDOWS)
    #include <windows.h>
#endif

namespace Birdy3d::core {

    template <>
    bool ResourceHandle<render::Shader>::load(ResourceIdentifier const& new_id)
    {
        auto val = ResourceManager::get_shader_ptr(new_id);
        if (!val)
            return false;
        m_ptr = val;
        return true;
    }

    template <>
    bool ResourceHandle<ui::Theme>::load(ResourceIdentifier const& new_id)
    {
        auto val = ResourceManager::get_theme_ptr(new_id);
        if (!val)
            return false;
        m_ptr = val;
        return true;
    }

    template <>
    bool ResourceHandle<render::Model>::load(ResourceIdentifier const& new_id)
    {
        auto val = ResourceManager::get_model_ptr(new_id);
        if (!val)
            return false;
        m_ptr = val;
        return true;
    }

    template <>
    bool ResourceHandle<render::Texture>::load(ResourceIdentifier const& new_id)
    {
        auto val = ResourceManager::get_texture_ptr(new_id);
        if (!val)
            return false;
        m_ptr = val;
        return true;
    }

    template <>
    bool ResourceHandle<physics::Collider>::load(ResourceIdentifier const& new_id)
    {
        auto val = ResourceManager::get_collider_ptr(new_id);
        if (!val)
            return false;
        m_ptr = val;
        return true;
    }

    ResourceIdentifier::ResourceIdentifier(std::string full_name)
    {
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
                if (it->empty()) {
                    core::Logger::warn("Invalid ResourceIdentifier: '{}'", full_name);
                    continue;
                }
                auto equalpos = it->find_first_of('=');
                if (equalpos == std::string::npos || equalpos == 0) {
                    core::Logger::warn("Invalid ResourceIdentifier: '{}'", full_name);
                    continue;
                }
                auto key = it->substr(0, equalpos);
                auto value = it->substr(equalpos + 1);
                args[key] = value;
            }
        }
    }

    ResourceIdentifier::ResourceIdentifier(char const* full_name)
        : ResourceIdentifier(std::string{full_name}){};

    ResourceIdentifier::operator std::string() const
    {
        return source + "::" + name + std::accumulate(args.cbegin(), args.cend(), std::string{}, [](std::string const& sum, std::pair<std::string, std::string> const& element) { return sum + ":" + element.first + "=" + element.second; });
    }

    std::unordered_map<std::string, std::shared_ptr<render::Shader>> ResourceManager::m_shaders;
    std::unordered_map<std::string, std::shared_ptr<ui::Theme>> ResourceManager::m_themes;
    std::unordered_map<std::string, std::shared_ptr<render::Model>> ResourceManager::m_models;
    std::unordered_map<std::string, std::shared_ptr<render::Texture>> ResourceManager::m_textures;
    std::unordered_map<std::string, std::shared_ptr<physics::Collider>> ResourceManager::m_colliders;

    ResourceHandle<render::Shader> ResourceManager::get_shader(ResourceIdentifier const& id)
    {
        return ResourceHandle<render::Shader>(id);
    }

    ResourceHandle<ui::Theme> ResourceManager::get_theme(ResourceIdentifier const& id)
    {
        return ResourceHandle<ui::Theme>(id);
    }

    ResourceHandle<render::Model> ResourceManager::get_model(ResourceIdentifier const& id)
    {
        return ResourceHandle<render::Model>(id);
    }

    ResourceHandle<render::Texture> ResourceManager::get_texture(ResourceIdentifier const& id)
    {
        return ResourceHandle<render::Texture>(id);
    }

    ResourceHandle<physics::Collider> ResourceManager::get_collider(ResourceIdentifier const& id)
    {
        return ResourceHandle<physics::Collider>(id);
    }

    std::shared_ptr<render::Shader> ResourceManager::get_shader_ptr(ResourceIdentifier const& id)
    {
        std::string name = static_cast<std::string>(id);
        std::shared_ptr<render::Shader> shader = m_shaders[name];
        if (!shader) {
            if (id.source != "file" && id.source != "") {
                Logger::error("invalid shader source '{}'", id.source);
                return nullptr;
            }
            std::map<std::string, std::string> shader_parameters;
            for (auto const& [key, value] : id.args) {
                shader_parameters[key] = value;
            }
            shader = std::make_shared<render::Shader>(id.name, shader_parameters);
            m_shaders[name] = shader;
        }
        return shader;
    }

    std::shared_ptr<ui::Theme> ResourceManager::get_theme_ptr(ResourceIdentifier const& id)
    {
        std::string name = static_cast<std::string>(id);
        std::shared_ptr<ui::Theme> theme = m_themes[name];
        if (!theme) {
            if (id.source == "file" || id.source == "") {
                std::string path = get_resource_path(id.name, ResourceType::THEME);
                if (path.empty())
                    return nullptr;
                std::string file_content = ResourceManager::read_file(path);
                if (file_content.empty())
                    return nullptr;
                try {
                    theme = std::make_shared<ui::Theme>(file_content);
                } catch (std::exception const& e) {
                    return nullptr;
                }
            } else {
                Logger::error("invalid theme source '{}'", id.source);
            }
            m_themes[name] = theme;
        }
        return theme;
    }

    std::shared_ptr<render::Model> ResourceManager::get_model_ptr(ResourceIdentifier const& id)
    {
        std::string name = static_cast<std::string>(id);
        std::shared_ptr<render::Model> model = m_models[name];
        if (!model) {
            if (id.source == "file" || id.source == "") {
                std::string path = get_resource_path(id.name, ResourceType::MODEL);
                if (path.empty())
                    return nullptr;
                model = std::make_shared<render::Model>(path);
            } else if (id.source == "primitive") {
                if (id.name == "plane") {
                    int resolution = 1;
                    if (id.args.contains("resolution")) {
                        int resolution_arg = std::stoi(id.args.at("resolution"));
                        if (resolution_arg > resolution)
                            resolution = resolution_arg;
                    }
                    model = utils::PrimitiveGenerator::generate_plane(resolution);
                } else if (id.name == "cube") {
                    model = utils::PrimitiveGenerator::generate_cube();
                } else if (id.name == "uv_sphere") {
                    int resolution = 5;
                    if (id.args.contains("resolution")) {
                        int resolution_arg = std::stoi(id.args.at("resolution"));
                        if (resolution_arg > resolution)
                            resolution = resolution_arg;
                    }
                    model = utils::PrimitiveGenerator::generate_uv_sphere(resolution);
                } else if (id.name == "ico_sphere") {
                    int resolution = 5;
                    if (id.args.contains("resolution")) {
                        int resolution_arg = std::stoi(id.args.at("resolution"));
                        if (resolution_arg > resolution)
                            resolution = resolution_arg;
                    }
                    model = utils::PrimitiveGenerator::generate_ico_sphere(resolution);
                } else {
                    Logger::error("invalid primitive type '{}'", id.name);
                }
            } else {
                Logger::error("invalid model source '{}'", id.source);
            }
            m_models[name] = model;
        }
        return model;
    }

    std::shared_ptr<render::Texture> ResourceManager::get_texture_ptr(ResourceIdentifier const& id)
    {
        std::string name = static_cast<std::string>(id);
        std::shared_ptr<render::Texture> texture = m_textures[name];
        if (!texture) {
            if (id.source == "file" || id.source == "") {
                std::string path = get_resource_path(id.name, ResourceType::TEXTURE);
                if (path.empty())
                    return nullptr;
                texture = std::make_shared<render::Texture>(path);
            } else if (id.source == "color") {
                utils::Color color = id.name;
                texture = std::make_shared<render::Texture>(color);
            } else {
                Logger::error("invalid texture source '{}'", id.source);
            }
            m_textures[name] = texture;
        }
        return texture;
    }

    std::shared_ptr<physics::Collider> ResourceManager::get_collider_ptr(ResourceIdentifier const& id)
    {
        std::string name = static_cast<std::string>(id);
        auto collider = m_colliders[name];
        if (collider)
            return collider;

        auto model = get_model_ptr(id);
        if (!model)
            return {};

        if (id.source == "primitive") {
            if (id.name == "plane") {
                Logger::error("Can't generate a Collider for the plane primitive.");
                return {};
            } else if (id.name == "uv_sphere" || id.name == " ico_sphere") {
                auto shapes = std::vector<std::unique_ptr<physics::CollisionShape>>();
                shapes.push_back(std::make_unique<physics::CollisionSphere>(1.0f));
                collider = std::make_shared<physics::Collider>(model, std::move(shapes));
            }
        }

        if (!collider) {
            physics::GenerationMode generation_mode = physics::GenerationMode::NONE;
            if (!id.args.contains("generation_mode"))
                return {};

            auto mode_string = id.args.at("generation_mode");
            if (mode_string == "NONE") {
                generation_mode = physics::GenerationMode::NONE;
            } else if (mode_string == "COPY") {
                generation_mode = physics::GenerationMode::COPY;
            } else if (mode_string == "HULL_MODEL") {
                generation_mode = physics::GenerationMode::HULL_MODEL;
            } else if (mode_string == "HULL_MESHES") {
                generation_mode = physics::GenerationMode::HULL_MESHES;
            } else if (mode_string == "DECOMPOSITION_MODEL") {
                generation_mode = physics::GenerationMode::DECOMPOSITION_MODEL;
            } else if (mode_string == "DECOMPOSITION_MESHES") {
                generation_mode = physics::GenerationMode::DECOMPOSITION_MESHES;
            } else {
                Logger::error("Invalid generation mode '{}'", mode_string);
                return {};
            }

            collider = physics::ConvexMeshGenerators::generate_collider(generation_mode, model);
        }

        m_colliders[name] = collider;
        return collider;
    }

    std::string ResourceManager::get_resource_path(std::string name, ResourceType type)
    {
        if (name.size() == 0) {
            Logger::error("invalid resource name");
            return {};
        }

        std::replace(name.begin(), name.end(), '\\', '/');

        std::string default_dir;
        std::string subdir;
        switch (type) {
        case ResourceType::SHADER:
            default_dir = "../shaders/";
            subdir = "shaders/";
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
            break;
        default:
            return {};
        }

        auto possible_paths = {
            name,
            get_resource_dir() + name,
            get_resource_dir() + subdir + name,
            get_resource_dir() + default_dir + name};

        for (auto path : possible_paths) {
            if (std::filesystem::is_regular_file(path))
                return path;
        }

        Logger::error("can't find resource '{}'", name);
        return {};
    }

    std::string ResourceManager::get_executable_dir()
    {
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

    std::string ResourceManager::get_resource_dir()
    {
        static auto const RESOURCE_DIR = get_executable_dir() + "../resources/";
        return RESOURCE_DIR;
    }

    std::string ResourceManager::read_file(std::string const& path, bool convert_eol)
    {
        std::ifstream file_stream;
        std::string content;
        try {
            file_stream.open(path);
            content.assign(std::istreambuf_iterator<char>(file_stream), std::istreambuf_iterator<char>());
            if (convert_eol)
                content.erase(std::remove(content.begin(), content.end(), '\r'), content.end());
        } catch (std::ifstream::failure& e) {
            Logger::error("Failed to read file '{}'", path);
        }
        return content;
    }

}
