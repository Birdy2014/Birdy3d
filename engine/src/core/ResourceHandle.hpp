#pragma once

#include "core/Application.hpp"
#include "events/EventBus.hpp"
#include "events/ResourceEvents.hpp"
#include "physics/Forward.hpp"
#include "render/Forward.hpp"
#include "ui/Forward.hpp"
#include "utils/serializer/Adapter.hpp"
#include <memory>
#include <string>

namespace Birdy3d::core {

    enum class ResourceType {
        SHADER,
        TEXTURE,
        THEME,
        MODEL,
        FONT
    };

    class ResourceIdentifier {
    public:
        ResourceType type;
        std::string source;
        std::string name;
        std::map<std::string, std::string> args;

        ResourceIdentifier() = default;
        ResourceIdentifier(std::string);
        ResourceIdentifier(char const* str);
        bool operator==(ResourceIdentifier const&) const;
        [[nodiscard]] std::string to_string(bool include_args = true) const;
    };

    template <class T>
    class ResourceHandle {
    public:
        ResourceHandle() = default;
        ResourceHandle(ResourceIdentifier id)
            : m_resource_id(id)
        {
            load(m_resource_id);
        }
        T const& operator*() const { return *ptr(); }
        T const* operator->() const { return ptr(); }
        explicit operator std::string() const { return m_resource_id.to_string(); }
        explicit operator bool() const { return static_cast<bool>(ptr()); }
        bool load() { return load(m_resource_id); }
        ResourceIdentifier const& id() { return m_resource_id; }
        [[nodiscard]] T const* ptr() const { return nullptr; }

        bool operator=(ResourceIdentifier new_id)
        {
            return load(new_id);
        }

        std::string arg(std::string key)
        {
            return m_resource_id.args[key];
        }

        void arg(std::string key, std::string value)
        {
            auto old = m_resource_id.args[key];
            m_resource_id.args[key] = value;
            if (old != value)
                load();
        }

        void arg(std::string key, std::integral auto value)
        {
            arg(key, std::to_string(value));
        }

    private:
        ResourceIdentifier m_resource_id;
        std::optional<std::size_t> mutable m_resource_index;
        std::optional<std::size_t> mutable m_new_resource_index;

        bool load(ResourceIdentifier const&) { return false; }

        void notify_load()
        {
            core::Application::event_bus->emit<events::ResourceLoadEvent>();
        }
    };

    template <>
    [[nodiscard]] render::Shader const* ResourceHandle<render::Shader>::ptr() const;
    template <>
    [[nodiscard]] ui::Theme const* ResourceHandle<ui::Theme>::ptr() const;
    template <>
    [[nodiscard]] render::Model const* ResourceHandle<render::Model>::ptr() const;
    template <>
    [[nodiscard]] render::Texture const* ResourceHandle<render::Texture>::ptr() const;
    template <>
    [[nodiscard]] physics::Collider const* ResourceHandle<physics::Collider>::ptr() const;

    template <>
    bool ResourceHandle<render::Shader>::load(ResourceIdentifier const& new_id);
    template <>
    bool ResourceHandle<ui::Theme>::load(ResourceIdentifier const& new_id);
    template <>
    bool ResourceHandle<render::Model>::load(ResourceIdentifier const& new_name);
    template <>
    bool ResourceHandle<render::Texture>::load(ResourceIdentifier const& new_name);
    template <>
    bool ResourceHandle<physics::Collider>::load(ResourceIdentifier const& new_name);

}

namespace Birdy3d::serializer {

    template <class T>
    Value adapter_save(core::ResourceHandle<T>& value)
    {
        return String((std::string)value);
    }

    template <class T>
    void adapter_load(Value* from, core::ResourceHandle<T>& to)
    {
        if (auto* string_ptr = std::get_if<String>(from)) {
            to = core::ResourceHandle<T>(string_ptr->value);
        }
    }

}
