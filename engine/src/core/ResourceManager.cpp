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
    [[nodiscard]] render::Shader const* ResourceHandle<render::Shader>::ptr() const
    {
        if (m_new_resource_index.has_value()) {
            auto resource = ResourceManager::get_shader_ptr(m_new_resource_index.value());
            if (resource) {
                m_resource_index = m_new_resource_index.value();
                m_new_resource_index = {};
                return resource;
            }
        }

        if (m_resource_index.has_value())
            return ResourceManager::get_shader_ptr(m_resource_index.value());

        return nullptr;
    }

    template <>
    [[nodiscard]] ui::Theme const* ResourceHandle<ui::Theme>::ptr() const
    {
        if (m_new_resource_index.has_value()) {
            auto resource = ResourceManager::get_theme_ptr(m_new_resource_index.value());
            if (resource) {
                m_resource_index = m_new_resource_index.value();
                m_new_resource_index = {};
                return resource;
            }
        }

        if (m_resource_index.has_value())
            return ResourceManager::get_theme_ptr(m_resource_index.value());

        return nullptr;
    }

    template <>
    [[nodiscard]] render::Model const* ResourceHandle<render::Model>::ptr() const
    {
        if (m_new_resource_index.has_value()) {
            auto resource = ResourceManager::get_model_ptr(m_new_resource_index.value());
            if (resource) {
                m_resource_index = m_new_resource_index.value();
                m_new_resource_index = {};
                return resource;
            }
        }

        if (m_resource_index.has_value())
            return ResourceManager::get_model_ptr(m_resource_index.value());

        return nullptr;
    }

    template <>
    [[nodiscard]] render::Texture const* ResourceHandle<render::Texture>::ptr() const
    {
        if (m_new_resource_index.has_value()) {
            auto resource = ResourceManager::get_texture_ptr(m_new_resource_index.value());
            if (resource) {
                m_resource_index = m_new_resource_index.value();
                m_new_resource_index = {};
                return resource;
            }
        }

        if (m_resource_index.has_value())
            return ResourceManager::get_texture_ptr(m_resource_index.value());

        return nullptr;
    }

    template <>
    [[nodiscard]] physics::Collider const* ResourceHandle<physics::Collider>::ptr() const
    {
        if (m_new_resource_index.has_value()) {
            auto resource = ResourceManager::get_collider_ptr(m_new_resource_index.value());
            if (resource) {
                m_resource_index = m_new_resource_index.value();
                m_new_resource_index = {};
                return resource;
            }
        }

        if (m_resource_index.has_value())
            return ResourceManager::get_collider_ptr(m_resource_index.value());

        return nullptr;
    }

    template <>
    bool ResourceHandle<render::Shader>::load(ResourceIdentifier const& new_id)
    {
        auto optional_index = ResourceManager::load_shader_ptr(new_id);
        if (!optional_index.has_value())
            return false;
        m_resource_id = new_id;
        if (m_new_resource_index.has_value() && ResourceManager::get_shader_ptr(m_new_resource_index.value()))
            m_resource_index = m_new_resource_index.value();
        m_new_resource_index = optional_index.value();
        notify_load();
        return true;
    }

    template <>
    bool ResourceHandle<ui::Theme>::load(ResourceIdentifier const& new_id)
    {
        auto optional_index = ResourceManager::load_theme_ptr(new_id);
        if (!optional_index.has_value())
            return false;
        m_resource_id = new_id;
        if (m_new_resource_index.has_value() && ResourceManager::get_theme_ptr(m_new_resource_index.value()))
            m_resource_index = m_new_resource_index.value();
        m_new_resource_index = optional_index.value();
        notify_load();
        return true;
    }

    template <>
    bool ResourceHandle<render::Model>::load(ResourceIdentifier const& new_id)
    {
        auto optional_index = ResourceManager::load_model_ptr(new_id);
        if (!optional_index.has_value())
            return false;
        m_resource_id = new_id;
        if (m_new_resource_index.has_value() && ResourceManager::get_model_ptr(m_new_resource_index.value()))
            m_resource_index = m_new_resource_index.value();
        m_new_resource_index = optional_index.value();
        notify_load();
        return true;
    }

    template <>
    bool ResourceHandle<render::Texture>::load(ResourceIdentifier const& new_id)
    {
        auto optional_index = ResourceManager::load_texture_ptr(new_id);
        if (!optional_index.has_value())
            return false;
        m_resource_id = new_id;
        if (m_new_resource_index.has_value() && ResourceManager::get_texture_ptr(m_new_resource_index.value()))
            m_resource_index = m_new_resource_index.value();
        m_new_resource_index = optional_index.value();
        return true;
    }

    template <>
    bool ResourceHandle<physics::Collider>::load(ResourceIdentifier const& new_id)
    {
        auto optional_index = ResourceManager::load_collider_ptr(new_id);
        if (!optional_index.has_value())
            return false;
        m_resource_id = new_id;
        if (m_new_resource_index.has_value() && ResourceManager::get_collider_ptr(m_new_resource_index.value()))
            m_resource_index = m_new_resource_index.value();
        m_new_resource_index = optional_index.value();
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

    bool ResourceIdentifier::operator==(ResourceIdentifier const& other) const
    {
        return type == other.type
            && source == other.source
            && name == other.name
            && args == other.args;
    }

    std::string ResourceIdentifier::to_string(bool include_args) const
    {
        auto location = source + "::" + name;
        if (!include_args)
            return location;

        auto data = std::accumulate(args.cbegin(), args.cend(), std::string{}, [](std::string const& sum, std::pair<std::string, std::string> const& element) { return sum + ":" + element.first + "=" + element.second; });

        return location + data;
    }

    std::unordered_map<std::string, std::size_t> ResourceManager::m_shader_indices;
    std::unordered_map<std::string, std::size_t> ResourceManager::m_theme_indices;
    std::unordered_map<std::string, std::size_t> ResourceManager::m_model_indices;
    std::unordered_map<std::string, std::size_t> ResourceManager::m_texture_indices;
    std::unordered_map<std::string, std::size_t> ResourceManager::m_collider_indices;

    std::vector<std::unique_ptr<render::Shader>> ResourceManager::m_shaders;
    std::vector<std::unique_ptr<ui::Theme>> ResourceManager::m_themes;
    std::vector<std::unique_ptr<render::Model>> ResourceManager::m_models;
    std::vector<std::unique_ptr<render::Texture>> ResourceManager::m_textures;
    std::vector<std::unique_ptr<physics::Collider>> ResourceManager::m_colliders;

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

    std::optional<std::size_t> ResourceManager::load_shader_ptr(ResourceIdentifier const& id)
    {
        auto name = id.to_string();

        if (m_shader_indices.contains(name))
            return m_shader_indices[name];

        if (id.source != "file" && id.source != "") {
            Logger::error("invalid shader source '{}'", id.source);
            return {};
        }

        std::map<std::string, std::string> shader_parameters;
        for (auto const& [key, value] : id.args) {
            shader_parameters[key] = value;
        }

        auto shader = std::make_unique<render::Shader>(id.name, shader_parameters);

        auto index = m_shaders.size();
        m_shader_indices[name] = index;
        m_shaders.push_back(std::move(shader));

        return index;
    }

    std::optional<std::size_t> ResourceManager::load_theme_ptr(ResourceIdentifier const& id)
    {
        std::string name = id.to_string();

        if (m_theme_indices.contains(name))
            return m_theme_indices[name];

        if (id.source == "file" || id.source == "") {
            std::string path = get_resource_path(id.name, ResourceType::THEME);
            if (path.empty())
                return {};
            std::string file_content = ResourceManager::read_file(path);
            if (file_content.empty())
                return {};
            try {
                auto theme = std::make_unique<ui::Theme>(file_content);

                auto index = m_themes.size();
                m_theme_indices[name] = index;
                m_themes.push_back(std::move(theme));

                return index;
            } catch (std::exception const& e) {
                return {};
            }
        } else {
            Logger::error("invalid theme source '{}'", id.source);
        }

        return {};
    }

    std::optional<std::size_t> ResourceManager::load_model_ptr(ResourceIdentifier const& id)
    {
        std::string name = id.to_string();

        if (m_model_indices.contains(name))
            return m_model_indices[name];

        std::unique_ptr<render::Model> model;

        if (id.source == "file" || id.source == "") {
            std::string path = get_resource_path(id.name, ResourceType::MODEL);
            if (path.empty())
                return {};
            model = std::make_unique<render::Model>(path);
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
                return {};
            }
        } else {
            Logger::error("invalid model source '{}'", id.source);
            return {};
        }

        auto index = m_models.size();
        m_model_indices[name] = index;
        m_models.push_back(std::move(model));

        return index;
    }

    std::optional<std::size_t> ResourceManager::load_texture_ptr(ResourceIdentifier const& id)
    {
        std::string name = id.to_string();

        if (m_texture_indices.contains(name))
            return m_texture_indices[name];

        if (id.source == "file" || id.source == "") {
            std::string path = get_resource_path(id.name, ResourceType::TEXTURE);
            if (path.empty())
                return {};

            auto index = m_textures.size();
            m_texture_indices[name] = index;
            m_textures.push_back(nullptr);

            core::Application::defer_loading([=]() {
                auto optional_image = utils::TextureLoader::from_file(path);

                if (!optional_image.has_value()) {
                    core::Logger::warn("Failed to load texture at {}", path);
                    return;
                }

                core::Application::defer_main([index, image = optional_image.value()]() {
                    auto texture = std::make_unique<render::Texture>(image);

                    if (!m_textures[index])
                        m_textures[index] = std::move(texture);

                    core::Application::event_bus->emit<events::ResourceLoadEvent>();
                });
            });

            return index;
        } else if (id.source == "color") {
            utils::Color color = id.name;
            auto texture = std::make_unique<render::Texture>(color);

            auto index = m_textures.size();
            m_texture_indices[name] = index;
            m_textures.push_back(std::move(texture));

            return index;
        } else {
            Logger::error("invalid texture source '{}'", id.source);
        }

        return {};
    }

    std::optional<std::size_t> ResourceManager::load_collider_ptr(ResourceIdentifier const& id)
    {
        std::string name = id.to_string(false);

        if (m_collider_indices.contains(name))
            return m_collider_indices[name];

        auto model_index = load_model_ptr(id);
        if (!model_index.has_value())
            return {};

        // FIXME: Wait for model to be available
        auto model = get_model_ptr(model_index.value());
        if (!model)
            return {};

        if (id.source == "primitive") {
            if (id.name == "plane") {
                Logger::error("Can't generate a Collider for the plane primitive.");
                return {};
            } else if (id.name == "uv_sphere" || id.name == " ico_sphere") {
                auto shapes = std::vector<std::unique_ptr<physics::CollisionShape>>();
                shapes.push_back(std::make_unique<physics::CollisionSphere>(1.0f));
                auto collider = std::make_unique<physics::Collider>(std::move(shapes));

                auto index = m_colliders.size();
                m_collider_indices[name] = index;
                m_colliders.push_back(std::move(collider));
                return index;
            }
        }

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

        auto index = m_colliders.size();
        m_collider_indices[name] = index;
        m_colliders.push_back(nullptr);

        core::Application::defer_loading([=]() {
            auto collider = physics::ConvexMeshGenerators::generate_collider(generation_mode, *model);

            // FIXME: Possible race condition. Use a mutex?
            if (!m_colliders[index])
                m_colliders[index] = std::move(collider);

            core::Application::event_bus->emit<events::ResourceLoadEvent>();
        });

        return index;
    }

    render::Shader const* ResourceManager::get_shader_ptr(std::size_t const& index)
    {
        if (index >= m_shaders.size())
            return nullptr;
        return m_shaders[index].get();
    }

    ui::Theme const* ResourceManager::get_theme_ptr(std::size_t const& index)
    {
        if (index >= m_themes.size())
            return nullptr;
        return m_themes[index].get();
    }

    render::Model const* ResourceManager::get_model_ptr(std::size_t const& index)
    {
        if (index >= m_models.size())
            return nullptr;
        return m_models[index].get();
    }

    render::Texture const* ResourceManager::get_texture_ptr(std::size_t const& index)
    {
        if (index >= m_textures.size())
            return nullptr;
        return m_textures[index].get();
    }

    physics::Collider const* ResourceManager::get_collider_ptr(std::size_t const& index)
    {
        if (index >= m_colliders.size())
            return nullptr;
        return m_colliders[index].get();
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
