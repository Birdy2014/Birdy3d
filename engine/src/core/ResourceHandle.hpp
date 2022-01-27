#pragma once

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
        explicit operator std::string() const;
    };

    template <class T>
    class ResourceHandle {
    public:
        ResourceHandle() = default;
        ResourceHandle(ResourceIdentifier id)
            : m_id(id) { load(m_id); }
        T& operator*() const { return *m_ptr; }
        T* operator->() const { return m_ptr.get(); }
        explicit operator std::string() const { return static_cast<std::string>(m_id); }
        explicit operator bool() const { return static_cast<bool>(m_ptr); }
        std::shared_ptr<T> ptr() const { return m_ptr; }
        bool load() { return load(m_id); }

        bool operator=(ResourceIdentifier new_id) {
            auto success = load(new_id);
            if (success)
                m_id = new_id;
            return success;
        }

        std::string arg(std::string key) {
            return m_id.args[key];
        }

        void arg(std::string key, std::string value) {
            m_id.args[key] = value;
        }

        void arg(std::string key, std::integral auto value) {
            m_id.args[key] = std::to_string(value);
        }

    private:
        ResourceIdentifier m_id;
        std::shared_ptr<T> m_ptr;

        bool load(const ResourceIdentifier&) { return false; }
    };

    template <>
    bool ResourceHandle<render::Shader>::load(const ResourceIdentifier& new_id);
    template <>
    bool ResourceHandle<ui::Theme>::load(const ResourceIdentifier& new_id);
    template <>
    bool ResourceHandle<render::Model>::load(const ResourceIdentifier& new_name);
    template <>
    bool ResourceHandle<render::Texture>::load(const ResourceIdentifier& new_name);

}

namespace Birdy3d::serializer {

    template <class T>
    Value adapter_save(core::ResourceHandle<T>& value) {
        return String((std::string)value);
    }

    template <class T>
    void adapter_load(Value* from, core::ResourceHandle<T>& to) {
        if (auto* string_ptr = std::get_if<String>(from)) {
            to = core::ResourceHandle<T>(string_ptr->value);
        }
    }

}
